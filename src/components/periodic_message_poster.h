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

#ifndef PERIODIC_MESSAGE_POSTER_H
#define PERIODIC_MESSAGE_POSTER_H

#include <Arduino.h>
#include "ticker_component.h"
#include "mqtt_handler.h"


class periodic_message_poster: public ticker_component
{
private:
    String topic;
    String message;
    unsigned long interval;
    unsigned long last;
    mqtt_handler *mqtt;

public:
    void update();
    void begin(mqtt_handler* mqtt, const String& topic, const String& message, unsigned long interval);
    void post_now();
    void set_message(const String &message_) { message = message_; }
};

#endif
