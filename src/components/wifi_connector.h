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

#ifndef WIFI_CONNECTOR_H
#define WIFI_CONNECTOR_H

#include <Arduino.h>
#include <functional>
#include "ticker_component.h"
#include <IPAddress.h>

class wifi_connector: public ticker_component
{
private:
    String ssid, password;
    bool connecting;
    bool connected;
public:
    typedef std::function<void()> callback;
    
    wifi_connector();
    void begin(const String& ssid, const String& password);
    void update();
    
    bool is_connected() const;
    
    callback on_connected;
    callback on_disconnected;

    bool host_by_name(String hostname, IPAddress &ip);
};

#endif // WIFI_CONNECTOR_H
