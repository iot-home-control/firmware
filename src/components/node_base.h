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

#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>

#include "config.h"
#include "wifi_connector.h"
#include "updater_ota.h"
#include "mqtt_handler.h"
#include "periodic_message_poster.h"

#if !defined(HAS_CONFIG_H)
#error "config.h not found!"
#endif

class node_base
{
public:
    node_base(bool serial_alternate_rx_tx=false);
    virtual ~node_base();
    virtual void setup();
    virtual void loop();
    void wait_for_loop_timing();
    String get_state_topic(const String& type, const int vnode=0);
    String get_action_topic(const String& type, const int vnode=0, const String& alt_suffix="");

    mqtt_handler mqtt;
protected:
    String device_id;
    wifi_connector wifi;
    updater_ota ota;
    ESP8266WebServer webserver;
    const float ticks_per_second=30.0f;
    unsigned long cycle_time_ms=(int)(1000/ticks_per_second);
    std::vector<ticker_component*> components;

    virtual void on_wifi_connected();
    virtual void on_wifi_disconnected();

    virtual void on_mqtt_connected();
    virtual void on_mqtt_disconnected();

private:
    unsigned long loop_start_ms;
    periodic_message_poster message_poster;
    bool serial_alternate_pins;
};
