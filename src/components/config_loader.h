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

#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>
#include <map>
#include <vector>


class ticker_component;

class config_loader
{
public:
    typedef std::function<ticker_component*(const JsonObject&/*obj*/, const int/*vnode_id*/)> FactoryFunction;
    void begin(std::vector<ticker_component*> &components, bool skip_components=false);
    String dump();
    // Returns true if reboot is required
    bool check(const String &device_id, const String &config_server="") const;
    bool install_factory(const char* type, int type_id, FactoryFunction creator);
private:
    struct factory
    {
        const char* type;
        const int type_id;
        FactoryFunction creator;
    };
    std::map<int, int> type_vnode_ids;
    std::vector<factory> factories;
    int current_config_version = -1;
    bool have_config_json = false;
};
