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

#include "node_base.h"

extern const char *build_date;
extern const char *git_version;

node_base::node_base(bool serial_alternate_rx_tx): serial_alternate_pins(serial_alternate_rx_tx)
{
    String id = String(ESP.getChipId(), HEX);

    while(id.length()<6)
        id = "0"+id;
    device_id="esp8266-"+id;
}

node_base::~node_base()
{
}

void node_base::setup() {
    // Init Hardware Serial on alternate Pins (RX:15/TX:13) instead of the
    // default pins, as the default RX pin is also I2S aka DMA ouput, which is
    // used by the LEDs
    Serial.begin(115200);
    if(serial_alternate_pins)
        Serial.pins(15,13);

    // Wait for the Serial port to become ready
    while(!Serial)
    {
        delay(10);
    }
    Serial.println();

    Serial.print("Built on ");
    Serial.print(build_date);
    Serial.print(" from git version ");
    Serial.println(git_version);

    Serial.print("Device-Id: ");
    Serial.println(device_id);

    wifi.begin(WIFI_SSID, WIFI_PASS);
    wifi.on_connected=std::bind(&node_base::on_wifi_connected, this);
    wifi.on_disconnected=std::bind(&node_base::on_wifi_disconnected, this);

    ota.set_password(OTA_PASS);

    mqtt.on_connected=std::bind(&node_base::on_mqtt_connected, this);
    mqtt.on_disconnected=std::bind(&node_base::on_mqtt_disconnected, this);

    message_poster.begin(&mqtt, "alive", device_id, 60000);
    components.push_back(&message_poster);

    webserver.begin(80);
    webserver.on("/reboot", [this]{
        webserver.send(200, "text/plain", "Ok");
        delay(100);
        ESP.reset();
    });
}

void node_base::loop() {
    loop_start_ms=millis();
    wifi.update();
    ota.update();
    mqtt.update();
    for(auto& component: components)
    {
        component->update();
    }
    webserver.handleClient();
}

void node_base::wait_for_loop_timing()
{
    unsigned long used_ms=millis()-loop_start_ms;
    if(used_ms<cycle_time_ms)
        delay(cycle_time_ms-used_ms);
}

String node_base::get_state_topic(const String &type, const int vnode)
{
    return type+"/"+device_id+"-"+vnode+"/state";
}

String node_base::get_action_topic(const String &type, const int vnode, const String &alt_suffix)
{
    String base = type+"/"+device_id+"-"+vnode;
    if(alt_suffix=="")
    {
        return base+"/action";
    }
    else
    {
        return base+"/"+alt_suffix;
    }

}

void node_base::on_wifi_connected()
{
    Serial.print("WiFi Connected (");
    Serial.print(WIFI_SSID);
    Serial.println(")");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    ota.begin();

    IPAddress mqtt_ip;
    if(wifi.host_by_name(MQTT_HOST, mqtt_ip)) {
        Serial.print("Resolved MQTT Hostname ");
        Serial.print(MQTT_HOST);
        Serial.print(" = ");
        Serial.println(mqtt_ip);
    } else {
        Serial.println("MQTT DNS query failed using MQTT_HOST as IP");
        mqtt_ip = mqtt_ip.fromString(MQTT_HOST);
    }

    mqtt.begin(mqtt_ip,device_id,MQTT_USER,MQTT_PASS);
}

void node_base::on_wifi_disconnected()
{
    Serial.println("WiFi Disconnected");
}

void node_base::on_mqtt_connected()
{
    Serial.println("MQTT Connected");
    message_poster.post_now();
}

void node_base::on_mqtt_disconnected()
{
    Serial.println("MQTT Disconnected");
}

