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

#include "wifi_connector.h"
#include <ESP8266WiFi.h>
#include <IPAddress.h>

wifi_connector::wifi_connector(): connecting(false), connected(false)
{
    WiFi.mode(WIFI_STA);
}

void wifi_connector::begin(const String& ssid, const String& password)
{
    this->ssid=ssid;
    this->password=password;
    connecting=true;
    WiFi.hostname("esp8266-"+String(ESP.getChipId(), HEX));
    WiFi.begin(this->ssid.c_str(), this->password.c_str());
}

void wifi_connector::update()
{
    auto status=WiFi.status();
    if(status==WL_CONNECTED)
    {
        if(connecting)
        {
            connecting=false;
            connected=true;
            if(on_connected)
                on_connected();
        }
    }
    else
    {
        if(connected)
        {
            // We were connected but aren't anymore
            connected=false;
            if(on_disconnected)
                on_disconnected();
        }
        if(!connecting)
        {
            // Start re-connecting
            connecting=true;
            WiFi.begin(this->ssid.c_str(), this->password.c_str());
        }
    }
}

bool wifi_connector::is_connected() const
{
    return connected;
}

bool wifi_connector::host_by_name(String hostname, IPAddress &ip)
{
    int rc=WiFi.hostByName(hostname.c_str(), ip);
    return rc==1;
}
