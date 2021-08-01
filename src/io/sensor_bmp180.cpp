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

#include "sensor_bmp180.h"

sensor_bmp180::sensor_bmp180(): sensor(nullptr), prev_millis(0), last_temperature(0), last_abs_pressure(0), altitude(NAN)
{
}

void sensor_bmp180::begin(unsigned long update_every_ms, const bool always_notify, const float altitude)
{
    this->update_every_ms = update_every_ms;
    this->always_notify = always_notify;
    this->altitude = altitude;
    if(!sensor)
        sensor = new SFE_BMP180();
    sensor->begin();
}

void sensor_bmp180::update()
{
    unsigned long curr_millis=millis();
    if(curr_millis-prev_millis>=update_every_ms)
    {
        prev_millis=curr_millis;
        if(!sensor)
            return;

        int sensor_delay = sensor->startTemperature();
        if(!sensor_delay)
            return;
        delay(sensor_delay);

        double temperature;
        if(!sensor->getTemperature(temperature))
            return;

        sensor_delay = sensor->startPressure(3);
        if(!sensor_delay)
            return;
        delay(sensor_delay);

        double abs_pressure;
        if(!sensor->getPressure(abs_pressure, temperature))
            return;

        if(temperature != last_temperature || always_notify)
        {
            last_temperature = temperature;
            if(on_temperature_changed)
                on_temperature_changed(temperature);
        }

        if(abs_pressure != last_abs_pressure || always_notify)
        {
            last_abs_pressure = abs_pressure;
            if(on_pressure_changed)
                on_pressure_changed(abs_pressure, get_relative_pressure());
        }
    }
}

float sensor_bmp180::get_temperature()
{
    return last_temperature;
}

float sensor_bmp180::get_absolute_pressure()
{
    return last_abs_pressure;
}

float sensor_bmp180::get_relative_pressure()
{
    if(isnan(altitude))
        return 0.0f;
    return sensor->sealevel(last_abs_pressure, altitude);
}
