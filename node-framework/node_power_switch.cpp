#include <Arduino.h>

#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
//#include <ArduinoJson.h>

#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#include <memory>

#include "config.h"
#include "wifi_connector.h"
#include "updater_ota.h"
//#include "updater_http.h"
#include "mqtt_handler.h"

//#include "sensor_dht22.h"
#include "gpio_pin.h"
#include "periodic_message_poster.h"
//#include "led_strip.h"
//#include "rotary_encoder.h"
#include "button.h"
#include "util.h"

#ifndef HAS_CONFIG_H
#error "config.h not found!"
#endif

String client_id;
wifi_connector wifi_con;
updater_ota update_ota;
mqtt_handler mqtt;
periodic_message_poster message_poster;

button btn;

gpio_pin pin_relay;
gpio_pin pin_led;

bool switch_state=false;


float ticks_per_second=30.0f;
unsigned long cycle_time_ms=(int)(1000/ticks_per_second);

std::vector<ticker_component*> components;

void switch_relay_on() {
    pin_relay.write(HIGH);
    pin_led.write(LOW);
    switch_state = true;
}

void switch_relay_off() {
    pin_relay.write(LOW);
    pin_led.write(HIGH);
    switch_state = false;
}

 
bool state=false;

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

    Serial.print("Built on ");
    Serial.print(__DATE__);
    Serial.print(", ");
    Serial.println(__TIME__);
    client_id="esp8266-"+String(ESP.getChipId(), HEX);
    Serial.print("Client-Id: ");
    Serial.println(client_id);


    wifi_con.begin(WIFI_SSID, WIFI_PASS);
    wifi_con.on_connected=[&]
    {
        Serial.print("Connected to WiFi (");
        Serial.print(WIFI_SSID);
        Serial.println(")");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        update_ota.begin();
    };
    
    update_ota.set_password(OTA_PASS);
    update_ota.set_hostname("power-switch");
    
   /* update_http.on_update_ok=[]
    {
        Serial.println("Ok. Rebooting.");
    };
    update_http.on_update_error=[](const String& error_msg)
    {
        Serial.print("Error: ");
        Serial.println(error_msg);
    };
    update_http.on_update_up_to_date=[]
    {
        Serial.println("Already up to date");
    };*/

    mqtt.begin(MQTT_HOST,client_id,MQTT_USER,MQTT_PASS);
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
    
    message_poster.begin(&mqtt, "/alive", client_id, 60000);
    components.push_back(&message_poster);
    
    pin_relay.begin(5, gpio_pin::pin_out);
    components.push_back(&pin_relay);
    
    pin_led.begin(2, gpio_pin::pin_out);
    components.push_back(&pin_led);
    
    btn.begin(4, gpio_pin::pin_in);
    components.push_back(&btn);

    //mqtt.handle_topic("/test/msg",default_msg_handler);
    //mqtt.handle_topic("/test/msg2",default_msg_handler);
    
    /*
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
*/

    
    auto switch_action_msg_handler=[&](char* topic, unsigned char* data, unsigned int length)
    {
        (void)topic;
        char s[length+1];
        memcpy(s,data,length);
        s[length]=0;
        String data_str(s);
        if(data_str=="on")
        {
            Serial.println("[MQTT] Turning on.");
            mqtt.publish("/switch/"+client_id+"/state","mqtt,on");
            switch_relay_on();
        }
        else if(data_str=="off")
        {
            Serial.println("[MQTT] Turning off.");
            mqtt.publish("/switch/"+client_id+"/state","mqtt,off");
            switch_relay_off();
        }
    };
    
  

    mqtt.handle_topic("/switch/action", switch_action_msg_handler);
    mqtt.handle_topic("/switch/"+client_id+"/action", switch_action_msg_handler);
    
    
    btn.on_short_click([&]
    {
        if(state) {
            Serial.println("Switch off the relay");
            mqtt.publish("/switch/"+client_id+"/state","local,off");
            switch_relay_off();
            
        }
        else {
            Serial.println("Switch on the relay");
            mqtt.publish("/switch/"+client_id+"/state","local,on");
            switch_relay_on();
        }
        state = !state;
    });
    
    pin_led.write(HIGH);
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
