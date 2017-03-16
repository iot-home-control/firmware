#include <Arduino.h>

#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>

#include <memory>

#include "config.h"
#include "wifi_connector.h"
#include "updater_ota.h"
#include "mqtt_handler.h"

#include "sensor_dht22.h"
#include "gpio_pin.h"
#include "periodic_message_poster.h"
#include "led_strip.h"
#include "rotary_encoder.h"
#include "util.h"

#ifndef HAS_CONFIG_H
#error "config.h not found!"
#endif

String client_id;
wifi_connector wifi_con;
updater_ota update_ota;
mqtt_handler mqtt;
periodic_message_poster message_poster;

rotary_encoder encoder;
led_strip leds(LED_STRIP_LENGTH);

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
float relative_brightness = 1.0;

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
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        update_ota.begin();
    };
    
    update_ota.set_password(OTA_PASS);
    update_ota.set_hostname("led-strip");
    
    mqtt.begin(MQTT_HOST,client_id,MQTT_USER,MQTT_PASS);
    mqtt.on_connected=[&]
    {
        Serial.println("Connected to MQTT");
        mqtt.publish("/switch/"+client_id+"/state","local,reboot");
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
    message_poster.begin(&mqtt, "/alive", client_id, 60000);
    components.push_back(&message_poster);

    auto led_action_msg_handler=[&](char* topic, unsigned char* data, unsigned int length)
    {
        (void)topic;
        char s[length+1];
        memcpy(s,data,length);
        s[length]=0;
        String data_str(s);
        if(data_str=="on")
        {
            Serial.println("[MQTT] Turning on.");
            leds.turn_on(get_current_color(),500);
            mqtt.publish("/leds/"+client_id+"/state","mqtt,on");
        }
        else if(data_str=="off")
        {
            Serial.println("[MQTT] Turning off.");
            leds.turn_off(500);
            mqtt.publish("/leds/"+client_id+"/state","mqtt,off");
        }
        else if(data_str=="sunrise")
        {
            Serial.println("[MQTT] Starting Sunrise");
            leds.play_animation_sunrise(600000);
        }
        else if(data_str=="warp_start")
        {
            Serial.println("[MQTT] Starting WARP-CORE");
            leds.play_animation_warp_core(HslColor(0.55,1.0,0.4),2000,true);
        }
        else if(data_str=="warp_stop")
        {
            Serial.println("[MQTT] Starting WARP-CORE");
            leds.play_animation_warp_core(HslColor(0.55,1.0,0.4),2000,false);
        }
        else if(data_str=="rainbow_start")
        {
            Serial.println("[MQTT] Starting rainbow");
            leds.play_animation_rainbow(2500, true);
        }
        else if(data_str=="rainbow_stop")
        {
            Serial.println("[MQTT] Starting rainbow");
            leds.play_animation_rainbow(2500, false);
        }
        else if(data_str.startsWith("preset="))
        {
            unsigned int preset_num = data_str.substring(7).toInt();
            Serial.print("[MQTT] Setting preset ");
            Serial.println(preset_num);
            preset_index=clamp(0u,preset_num,ARRAY_COUNT(presets_hsl)-1);
            apply_preset();
        }
   

    };

    mqtt.handle_topic("/leds/"+client_id+"/action", led_action_msg_handler);
    mqtt.handle_topic("/leds/action", led_action_msg_handler);
    auto led_color_msg_handler=[&](char* topic, unsigned char* data, unsigned int length)
    {
        char s[length+1];
        memcpy(s,data,length);
        s[length]=0;
        String data_str(s);

        String the_topic(topic);
        if(the_topic.endsWith("rgb"))
        {
            int rgb[3]={};
            int color=0;
            size_t last_comma=0;
            while(color<3)
            {
                size_t comma = data_str.indexOf(',', last_comma);
                Serial.print("found comma at ");
                Serial.println(comma);
                if(comma!=-1)
                {
                    rgb[color]=data_str.substring(last_comma,comma).toInt();
                    last_comma=comma+1;
                    color++;
                }
                else if(comma==-1 && color==2)
                {
                    rgb[color]=data_str.substring(last_comma).toInt();
                    last_comma=comma+1;
                    color++;
                }
                else
                    break;
            }
            Serial.println("[MQTT] Setting color to ");
            Serial.print(rgb[0]);
            Serial.print(",");
            Serial.print(rgb[1]);
            Serial.print(",");
            Serial.println(rgb[2]);

            leds.fade_to_color(RgbColor((uint8_t)rgb[0],
                                        (uint8_t)rgb[1],
                                        (uint8_t)rgb[2]),
                               500);
        }
    };

    mqtt.handle_topic("/leds/color/rgb", led_color_msg_handler);
    mqtt.handle_topic("/leds/"+client_id+"/color/rgb", led_color_msg_handler);

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
        if(leds.is_playing_animation())
        {
            leds.stop_animation();
            return;
        }
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
            mqtt.publish("/leds/"+client_id+"/state","local,off");
        }
        else
        {
            Serial.println("Turning on.");
            leds.turn_on(get_current_color(),500);
            mqtt.publish("/leds/"+client_id+"/state","local,on");
        }
        state=!state;
    });

    encoder.btn.on_long_click([]
    {
        //leds.play_animation(std::make_shared<rainbow_animation>(500));
        //leds.rainbow(2000);
        //leds.play_animation(new test_animation());
        //leds.play_animation_rainbow(2500, true);
        //leds.play_animation_sunrise(10000);
        //leds.play_animation_warp_core(HslColor(0.55,1.0,0.4),2000,true);
    });

    components.push_back(&encoder);
    //leds.fade_to_color(colors[preset_index], 1000);
#if defined(LED_STRIP_ENABLE_ON_BOOT)
    leds.turn_on(get_current_color(), 500);
#endif
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
