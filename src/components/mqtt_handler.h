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

#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <functional>
#include <vector>
#include "ticker_component.h"

class mqtt_handler: public ticker_component
{
public:
    typedef std::function<void(char* topic, unsigned char* data, unsigned int len)> topic_handler;
    typedef std::function<void()> connection_state_callback;
private:
    struct handler_pair
    {
        String topic;
        topic_handler handler;
        bool subscribed;
    };
    
    WiFiClient wifi_client;
    PubSubClient client;
    bool connecting, connected;
    unsigned long connection_retry_interval, connecting_start_time;
    unsigned int  connection_retry_count;
    unsigned long subscription_check_interval, last_subscription_check;
    String mqtt_id, mqtt_user, mqtt_pass;
    std::vector<handler_pair> handlers;
    bool can_connect=false;

    void mqtt_callback(char* topic, uint8_t* data, unsigned int length);
    void check_subscriptions();

public:
    mqtt_handler();
    void begin(IPAddress ip, const String& client_id, const String& user, const String& pass);
    void update();
    void handle_topic(const String& name, topic_handler handler);
    void publish(const String& topic, const String& msg);

    connection_state_callback on_connected;
    connection_state_callback on_disconnected;
};

#endif // MQTT_HANDLER_H
