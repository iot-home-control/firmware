#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#include "config.h"
#include "wifi_connector.h"
#include "ota_handler.h"
#include "mqtt_handler.h"

#include "sensor_dht22.h"
#include "gpio_pin.h"

#ifndef HAS_CONFIG_H
#error "config.h not found!"
#endif

String client_id;
wifi_connector wifi_con;
ota_handler ota;
mqtt_handler mqtt;
sensor_dht22 dht22;
gpio_pin pin0;
gpio_pin pin5;

unsigned int ticks_per_second=30;
unsigned long wait_ms=1000/ticks_per_second;

char json_str[]=R"({"name":"some-node","config-version":1,"features":[{"type":"gpio","pin":0,"dir":"in_pullup","post-on-change":true,"topic":"/foo"},{"type":"gpio","pin":13,"dir":"output","post-on-change":true,"topic":"/foo"}]})";


void parse_config()
{
    DynamicJsonBuffer json;
    JsonObject& root=json.parseObject(json_str);
    Serial.print("name: ");
    Serial.println(root["name"].asString());
    Serial.print("config-version: ");
    Serial.println(root["config-version"].asString());

    JsonArray& features=root["features"].asArray();

    for(JsonObject& feature: features)
    {
        Serial.print("feature type: ");
        Serial.print(feature["type"].asString());
        feature.printTo(Serial);
        Serial.println();

        if(feature["type"]=="gpio")
        {
            int pin=feature["pin"];
            gpio_pin::pin_dir dir;
            if(feature["dir"]=="in")
                dir=gpio_pin::pin_in;
            else if(feature["dir"]=="in_pullup")
                dir=gpio_pin::pin_in_pullup;
            else
                dir=gpio_pin::pin_out;

            auto *p=new gpio_pin;
            p->begin(pin,dir);

            if((dir==gpio_pin::pin_in||dir==gpio_pin::pin_in_pullup) && (feature["post-on-change"]==true))
            {
                String topic=feature["topic"];
                p->on_changed=[topic](bool v)
                {
                    Serial.print("Changed to");
                    Serial.print(v);
                    mqtt.publish(topic,v?"on":"off");
                };
            }
        }
    }
}


void setup() {
    // Init Hardware Serial on alternate Pins (RX:15/TX:13) instead of the
    // default pins, as the default RX pin is also I2S aka DMA ouput, which is
    // used by the LEDs
    Serial.begin(115200);
    Serial.pins(15,13);

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
        ota.begin();
    };
    
    ota.set_password(OTA_PASS);
    ota.set_hostname("snd");
    
    mqtt.begin("10.1.0.10",client_id,"esp8266","esp8266");
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
    
    mqtt.handle_topic("/test/msg",default_msg_handler);
    mqtt.handle_topic("/test/msg2",default_msg_handler);
    
    dht22.begin(12,10000);
    dht22.on_temperature_changed=[&](float v)
    {
        Serial.print("Temperature changed ");
        Serial.println(v);
    };
    dht22.on_humidity_changed=[&](float v)
    {
        Serial.print("Humidity changed ");
        Serial.println(v);
    };
    
    pin0.begin(0,gpio_pin::pin_in_pullup);
    pin0.on_changed=[&](bool v)
    {
        Serial.print("Pin0 changed to ");
        Serial.println(v);
    };
    
    pin5.begin(5,gpio_pin::pin_in_pullup);
    pin5.on_changed=[&](bool v)
    {
        Serial.print("Pin5 changed to ");
        Serial.println(v);
    };

    parse_config();
}

void loop() {
    wifi_con.update();
    ota.update();
    mqtt.update();
    dht22.update();
    pin0.update();
    pin5.update();

    delay(wait_ms);
}
