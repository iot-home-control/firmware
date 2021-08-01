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

#ifndef SENSOR_DHT22_H
#define SENSOR_DHT22_H

#include "../components/ticker_component.h"
#include <DHT.h>
#include <functional>

class sensor_dht22: public ticker_component
{
private:
    class wrapper
    {
    public:
        DHT dht;
        wrapper(unsigned char pin): dht(pin,DHT22){}
    };
    
    wrapper *sensor;
    unsigned long prev_millis, update_every_ms;
    
    float last_temperature, last_humidity;
    bool always_notify;
public:
    typedef std::function<void(const float)> callback;
    
    sensor_dht22();
    void begin(unsigned char pin, unsigned long update_every_ms, const bool always_notify=false);
    void update() override;
    float get_temperature();
    float get_humidity();
    
    callback on_temperature_changed;
    callback on_humidity_changed;
};

#endif // SENSOR_DHT22_H
