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
#include <Adafruit_BME280.h>
#include <functional>

class sensor_bme280: public ticker_component
{
private:
    Adafruit_BME280 *sensor;
    unsigned long prev_millis, update_every_ms;

    float last_temperature, last_abs_pressure, last_humidity;
    float altitude;
    bool always_notify;
public:
    typedef std::function<void(const float)> callback;
    typedef std::function<void(const float, const float)> callback2; // absolute, sea-level relative pressure

    sensor_bme280();
    void begin(unsigned long update_every_ms, const bool always_notify=false, const float altitude=NAN);
    void update() override;
    float get_temperature();
    float get_humidity();
    float get_absolute_pressure();
    float get_relative_pressure();

    callback on_temperature_changed;
    callback on_humidity_changed;
    callback2 on_pressure_changed;
};
