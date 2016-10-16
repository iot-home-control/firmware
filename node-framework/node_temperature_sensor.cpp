// kate: hl C++;
// vim: filetype=c++: 

#include <Arduino.h>

#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>

#include <DallasTemperature.h>

#include "config.h"
#include "wifi_connector.h"
#include "updater_ota.h"
#include "mqtt_handler.h"

#include "sensor_dht22.h"

#ifndef HAS_CONFIG_H
#error "config.h not found!"
#endif

String client_id;
wifi_connector wifi_con;
updater_ota update_ota;
mqtt_handler mqtt;

sensor_dht22 dht22;

float ticks_per_second=30.0f;
unsigned long cycle_time_ms=(int)(1000/ticks_per_second);

std::vector<ticker_component*> components;

void setup() {
    // Init Hardware Serial on alternate Pins (RX:15/TX:13) instead of the
    // default pins, as the default RX pin is also I2S aka DMA ouput, which is
    // used by the LEDs
    Serial.begin(115200);
    //Serial.pins(15,13);

    // Wait for the Serial port to become ready
    while(!Serial)
    {
        delay(10);
    }
    Serial.println();

    client_id="node-"+String(ESP.getChipId());
    Serial.print("Client-Id: ");
    Serial.println(client_id);

    wifi_con.begin(WIFI_SSID, WIFI_PASS);
    wifi_con.on_connected=[&]
    {
        Serial.println("Connected to WiFi");
        update_ota.begin();
    };
    
    update_ota.set_password(OTA_PASS);
    update_ota.set_hostname("temp-sensor");
    
    mqtt.begin("10.1.0.16",client_id,"esp8266","esp8266");
    mqtt.on_connected=[&]
    {
        Serial.println("Connected to MQTT");
    };
    mqtt.on_disconnected=[&]
    {
        Serial.println("Disconnected from MQTT");
    };
    
    auto default_msg_handler=[](char* topic, unsigned char* data, unsigned int length)
    {
        Serial.print("Received message in topic ");
        Serial.print(topic);
        Serial.print(": '");
        char s[length+1];
        memcpy(s,data,length);
        s[length]=0;
        Serial.print(s);
        Serial.println("'");
    };
    
    dht22.begin(12,10000);
    dht22.on_temperature_changed=[&](float v)
    {
        Serial.print("Temperature changed ");
        Serial.println(v);
        String msg="{\"sensor\":\""+client_id+"\",\"temperature\":"+String(v)+"}";
        mqtt.publish("/sensor/livingroom/temperature",msg);
    };
    dht22.on_humidity_changed=[&](float v)
    {
        Serial.print("Humidity changed ");
        Serial.println(v);
        String msg="{\"sensor\":\""+client_id+"\",\"humidity\":"+String(v)+"}";
        mqtt.publish("/sensor/livingroom/humidity",msg);
    };

    components.push_back(&dht22);
}

void loop() {
    unsigned long start_ms=millis();
    wifi_con.update();
    update_ota.update();
    mqtt.update();
    for(auto& component: components)
    {
        component->update();
    }

    unsigned long used_ms=millis()-start_ms;
    if(used_ms<cycle_time_ms)
        delay(cycle_time_ms-used_ms);
}
