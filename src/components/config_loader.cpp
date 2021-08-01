// SPDX-License-Identifier: GPL-3.0-or-later
/*
    Copyright (C) 2021  The Home Control Authors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "config_loader.h"
#include "../components/ticker_component.h"
#include <LittleFS.h>
#include <ESP8266HTTPClient.h>
#include <algorithm>
#include <ESP8266WiFi.h>

#if 1
#define LOG(x) Serial.print(x)
#define LOGF(...) Serial.printf( __VA_ARGS__ )
#define LOGLN(x) Serial.println(x)
#else
#define LOG(...)
#define LOGF(...)
#define LOGLN(...)
#endif

template <template<class,class,class...> class C, typename K, typename V, typename... Args>
V GetWithDef(const C<K,V,Args...>& m, K const& key, const V & defval)
{
    typename C<K,V,Args...>::const_iterator it = m.find( key );
    if (it == m.end())
        return defval;
    return it->second;
}

void config_loader::begin(std::vector<ticker_component*> &components, bool skip_components)
{
    if(!LittleFS.begin())
    {
        LOGLN("[cl] Can't mount LittleFS.");
    }
    have_config_json = LittleFS.exists("/config.json");
    if(!have_config_json)
    {
        LOGLN("[cl] /config.json does not exist.");
        return;
    }
    File config_file = LittleFS.open("/config.json", "r");
    if(!config_file)
    {
        LOGLN("[cl] Can't open /config.json.");
        return;
    }
    DynamicJsonDocument jsonBuffer(512);
    auto errordescriptor = deserializeJson(jsonBuffer, config_file);
    if(errordescriptor != DeserializationError::Ok)
    {
        LOGLN("[cl] Could not parse /config.json.");
        return;
    }

    const auto& config = jsonBuffer.as<JsonObject>();
    if(!config.containsKey("version"))
    {
        LOGLN("[cl] Can't get version of existing config file");
        return;
    }
    current_config_version = config["version"].as<int>();

    if(!config.containsKey("components") || !config["components"].is<JsonArray>() || config["components"].as<JsonArray>().size()==0)
    {
        LOGLN("[cl] Config file does not contain any components. Key not found or not array or empty array.");
        return;
    }

    if(skip_components)
    {
        config_file.close();
        return;
    }

    const auto config_components = config["components"].as<JsonArray>();
    for(const auto& entry: config_components)
    {
        if(!entry.is<JsonObject>())
            continue;
        const JsonObject obj = entry.as<JsonObject>();
        if(!obj.containsKey("type"))
            continue;
        const char *obj_type = obj["type"].as<const char*>();

        const auto it=std::find_if(factories.cbegin(), factories.cend(), [&obj_type](const factory& f){ return !strcmp(f.type, obj_type); });
        if(it==factories.cend())
        {
            LOG("[cl] Can't find factory for type ");
            LOGLN(obj_type);
            continue;
        }
        if(!it->creator)
        {
            LOGF("[cl] Factory function for type %s is not set!\n", obj_type);
            continue;
        }

        int obj_type_id = it->type_id;
        int obj_vnode_id = GetWithDef(type_vnode_ids, obj_type_id, 0);
        LOGF("[cl] Found component! type=%s vnode_id=%d type_id=%i\n", obj_type, obj_vnode_id, obj_type_id);

        ticker_component *component = nullptr;
        component = it->creator(obj, obj_vnode_id);
        if(component == nullptr)
        {
            LOGLN("[cl] Factory function returned nullptr!!");
            continue;
        }
        type_vnode_ids[obj_type_id] = obj_vnode_id+1;
        components.push_back(component);
        LOGLN("[cl] Added component!");
    }

    LOGLN("[cl] Done parsing config file.");

    config_file.close();
}

String config_loader::dump()
{
    if(!LittleFS.begin())
    {
        LOGLN("[cl] Can't mount LittleFS.");
        return "cant-mount-spiffs";
    }
    have_config_json = LittleFS.exists("/config.json");
    if(!have_config_json)
    {
        LOGLN("[cl] /config.json does not exist.");
        return "no-config";
    }
    File config_file = LittleFS.open("/config.json", "r");
    if(!config_file)
    {
        LOGLN("[cl] Can't open /config.json.");
        return "cant-read-config";
    }
    String s = config_file.readString();
    config_file.close();
    return s;
}

bool config_loader::check(const String &device_id, const String &config_server) const
{
    WiFiClient wifi;
    const bool NO_REBOOT = false, DO_REBOOT = true;
    HTTPClient http;
    http.setTimeout(2000/*ms*/);
    http.useHTTP10(true);
    http.begin(wifi, config_server+F("/api/v1/config?device=")+device_id);
    int rc = http.GET();
    if(rc!=HTTP_CODE_OK)
    {
        if(rc<0)
            LOGF("[cl] http request failed: %i -> %s\n", rc, http.errorToString(rc).c_str());
        else
            LOGF("[cl] http error: %i\n", rc);
        return NO_REBOOT;
    }

    DynamicJsonDocument jsonBuffer(512);
    auto errordescriptor = deserializeJson(jsonBuffer, http.getStream());
    if(errordescriptor != DeserializationError::Ok)
    {
        LOGF("[cl] Could not parse downloaded config.json: %s\n", errordescriptor.c_str());
        return NO_REBOOT;
    }
    const auto config = jsonBuffer.as<JsonObject>();

    if(!config.containsKey("version"))
    {
        LOGLN("[cl] Can't get version of downloaded config file");
        return NO_REBOOT;
    }
    int downloaded_config_version = config["version"].as<int>();
    if(downloaded_config_version>current_config_version)
    {
        LOGF("[cl] Config was updated (%i -> %i). Saving and signaling for reboot\n", current_config_version, downloaded_config_version);
        File config_file = LittleFS.open("/config.json", "w+");
        if(!config_file)
        {
            LOGLN("[cl] Can't open /config.json for writing");
            return NO_REBOOT;
        }
        int written = serializeJson(config, config_file);
        LOGF("[cl] Wrote %i bytes to config.json\n", written);
        config_file.close();
        LittleFS.end();
        if(written<0)
            return NO_REBOOT;
        return DO_REBOOT;
    }
    else
    {
        LOGF("[cl] Config version is up to date: %i (remote: %i)\n", current_config_version, downloaded_config_version);
    }
    return NO_REBOOT;
}

bool config_loader::install_factory(const char* type, const int type_id, FactoryFunction creator)
{
    const auto it=std::find_if(factories.cbegin(), factories.cend(), [&type](const factory& f){ return !strcmp(f.type, type); });
    if(it==factories.cend())
    {
        factories.push_back({type, type_id, creator});
        return true;
    }
    return false;
}
