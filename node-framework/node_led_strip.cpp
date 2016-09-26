#include <Arduino.h>

#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#include "config.h"
#include "wifi_connector.h"
#include "updater_ota.h"
#include "updater_http.h"
#include "mqtt_handler.h"

#include "sensor_dht22.h"
#include "gpio_pin.h"
#include "led_strip.h"
#include "rotary_encoder.h"

#ifndef HAS_CONFIG_H
#error "config.h not found!"
#endif

String client_id;
wifi_connector wifi_con;
updater_ota update_ota;
updater_http update_http;
mqtt_handler mqtt;

rotary_encoder encoder;
led_strip leds(15);

float ticks_per_second=30.0f;
unsigned long cycle_time_ms=(int)(1000/ticks_per_second);

std::vector<ticker_component*> components;

RgbColor colors[]={
  {255, 147, 41}, //Candle (1900K)
  {255, 197, 143}, //40W Tungsten (2600K)
  {255, 214, 170}, //100W Tungsten (2850K)
  {255, 241, 224}, //Halogen (3200K)
  {255, 250, 244}, //Carbon Arc (5200K)
  {255, 255, 251}, //High Noon Sun (5400K)
  {255, 255, 255}, //Direct Sunlight (6000K)
  {201, 226, 255}, //Overcast Sky (7000K)
  {64, 156, 255}, //Clear Blue Sky (20000K)
};

HslColor presets_hsl[]={
  {0.0833, 1.0, 0.580}, //Candle (1900K)
  {0.0805, 1.0, 0.780}, //40W Tungsten (2600K)
  {0.1388, 1.0, 0.833}, //100W Tungsten (2850K)
  {0.0916, 1.0, 0.939}, //Halogen (3200K)
  {0.0916, 1.0, 0.978}, //Carbon Arc (5200K)
  {0.1666, 1.0, 0.992}, //High Noon Sun (5400K)
  {0.0000, 1.0, 1.000}, //Direct Sunlight (6000K)
  {0.4944, 1.0, 0.894}, //Overcast Sky (7000K)
  {0.5861, 1.0, 0.625}, //Clear Blue Sky (20000K)
};
unsigned int preset_index=0;
float relative_brightness = 0.1;

template<typename T>
T clamp(T min, T value, T max)
{
    if(value<min)
        return min;
    if(value>max)
        return max;
    return value;
}

HslColor get_current_color()
{
    HslColor c = presets_hsl[preset_index];
    c.L *= relative_brightness;

    return c;
}

void apply_preset(bool fade=true)
{
    if(!leds.is_on())
        return;

    HslColor c = get_current_color();
    if(fade)
        leds.fade_to_color(c, 500);
    else
        leds.set_color_to(c);
}



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

    leds.begin();
    components.push_back(&leds);

    wifi_con.begin(WIFI_SSID, WIFI_PASS);
    wifi_con.on_connected=[&]
    {
        Serial.print("Connected to WiFi (");
        Serial.print(WIFI_SSID);
        Serial.println(")");
        update_ota.begin();
    };
    
    update_ota.set_password(OTA_PASS);
    update_ota.set_hostname("led-strip");
    
    update_http.on_update_ok=[]
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
    };

    //mqtt.begin("10.1.0.10",client_id,"esp8266","esp8266");
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

    encoder.begin(5,4,0);
    encoder.on_encoder_changed([&](int delta)
    {
        relative_brightness+=delta*0.01;
        relative_brightness=clamp(0.0f, relative_brightness, 1.0f);
        Serial.print("Brightness: ");
        Serial.println(relative_brightness);
        apply_preset(false);
    });
    encoder.btn.on_short_click([]
    {
        preset_index=++preset_index%(ARRAY_COUNT(presets_hsl)-1);
        //leds.fade_to_color(presets_hsl[preset_index], 500);
        apply_preset();
        Serial.print("Preset:");
        Serial.println(preset_index);
    });
    encoder.btn.on_medium_click([]
    {
        static bool state=false;
        if(state)
        {
            Serial.println("Turning off.");
            leds.turn_off(500);
        }
        else
        {
            Serial.println("Turning on.");
            leds.turn_on(get_current_color(),500);
        }
        state=!state;
    });

    components.push_back(&encoder);
    //leds.fade_to_color(colors[preset_index], 1000);
    //apply_preset();
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
