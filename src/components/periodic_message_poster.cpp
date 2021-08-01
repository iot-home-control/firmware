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

#include "periodic_message_poster.h"

void periodic_message_poster::begin(mqtt_handler* mqtt, const String& topic, const String& message, unsigned long interval)
{
    this->topic = topic;
    this->message = message;
    this->interval = interval;
    this->last = millis();
    this->mqtt = mqtt;
}

void periodic_message_poster::update() 
{
    unsigned long now = millis();
    if (now - last > interval)
    {
        mqtt->publish(topic,message);
        last = now;
    }
}

void periodic_message_poster::post_now()
{
    mqtt->publish(topic,message);
    last = millis();
}
