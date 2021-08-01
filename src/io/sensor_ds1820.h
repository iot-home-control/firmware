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

#include "../components/ticker_component.h"

#include <functional>
#include <vector>
#include <DallasTemperature.h>
#include <OneWire.h>

class sensor_ds1820: public ticker_component
{
private:
    struct storable_onewire_address
    {
        DeviceAddress addr{0};
    };
    OneWire *onewire=nullptr;
    DallasTemperature dtlib;
    std::vector<storable_onewire_address> onewire_addresses;
    std::vector<float> last_temperatures;
    unsigned long prev_millis, update_every_ms;
    bool always_notify;
    int vnode_offset;
public:
    typedef std::function<void(const uint8_t /*sensor_index*/, const float /*temperature_in_deg_c*/, const int/*vnode_offset*/)> callback;

    sensor_ds1820();
    void begin(uint8_t pin, const unsigned long update_every_ms, const bool always_notify=false, const int vnode_offset=0);
    void update() override;

    callback on_temperature_changed;
};
