#include "config_loader.h"
#include "ticker_component.h"
#include <FS.h>
#include <ESP8266HTTPClient.h>
#include <algorithm>

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
    if(!SPIFFS.begin())
    {
        LOGLN("[cl] Can't mount SPIFFS.");
    }
    have_config_json = SPIFFS.exists("/config.json");
    if(!have_config_json)
    {
        LOGLN("[cl] /config.json does not exist.");
        return;
    }
    File config_file = SPIFFS.open("/config.json", "r");
    if(!config_file)
    {
        LOGLN("[cl] Can't open /config.json.");
        return;
    }
    DynamicJsonBuffer jsonBuffer;
    const auto& config = jsonBuffer.parseObject(config_file);
    if(!config.success())
    {
        LOGLN("[cl] Could not parse /config.json.");
        return;
    }
    
    if(!config.containsKey("version"))
    {
        LOGLN("[cl] Can't get version of existing config file");
        return;
    }
    current_config_version = config["version"].as<int>();

    if(!config.containsKey("components") || !config["components"].is<const JsonArray&>() || config["components"].as<const JsonArray&>().size()==0)
    {
        LOGLN("[cl] Config file does not contain any components. Key not found or not array or empty array.");
        return;
    }

    if(skip_components)
    {
        config_file.close();
        return;
    }

    const auto &config_components = config["components"].as<const JsonArray&>();
    for(const auto& entry: config_components)
    {
        if(!entry.is<const JsonObject&>())
            continue;
        const JsonObject &obj = entry.as<const JsonObject&>();
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

bool config_loader::check(const String &device_id, const String &config_server) const
{
    const bool NO_REBOOT = false, DO_REBOOT = true;
    HTTPClient http;
    http.setTimeout(500/*ms*/);
    http.begin(config_server+F("/api/v1/config?device=")+device_id);
    int rc = http.GET();
    if(rc!=HTTP_CODE_OK)
    {
        if(rc<0)
            LOGF("[cl] http request failed: %i -> %s\n", rc, http.errorToString(rc).c_str());
        else
            LOGF("[cl] http error: %i\n", rc);
        return NO_REBOOT;
    }
    
    DynamicJsonBuffer jsonBuffer;
    const auto& config = jsonBuffer.parseObject(http.getStream());
    
    if(!config.success())
    {
        LOGLN("[cl] Could not parse downloaded config.json");
        return NO_REBOOT;
    }

    if(!config.containsKey("version"))
    {
        LOGLN("[cl] Can't get version of downloaded config file");
        return NO_REBOOT;
    }
    int downloaded_config_version = config["version"].as<int>();
    if(downloaded_config_version>current_config_version)
    {
        LOGF("[cl] Config was updated (%i -> %i). Saving and signaling for reboot\n", current_config_version, downloaded_config_version);
        File config_file = SPIFFS.open("/config.json", "w+");
        if(!config_file)
        {
            LOGLN("[cl] Can't open /config.json for writing");
            return NO_REBOOT;
        }
        int written = config.printTo(config_file);
        LOGF("[cl] Wrote %i bytes to config.json\n", written);
        config_file.close();
        SPIFFS.end();
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
