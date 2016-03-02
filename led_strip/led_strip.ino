#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <RotaryEncoder.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <RgbColor.h>
#include <HslColor.h>
#include "data.h"
#include "ButtonControl.h"

#define led_count 15
//#define led_pin 13 // D4
#define button_pin 0 // D3
#define rotary_pin_left  5 // D1
#define rotary_pin_right 4 // D2
#define wifi_bit0 12 // Pullup
#define wifi_bit1 14 // Pullup: 11=Home, 10=Frefunk, 01=Rikus, 00=Reserved

#define ArrayCount(arr) (sizeof(arr)/sizeof(0[arr]))

NeoPixelBus leds(led_count, 0); // pin number not required with dma
NeoPixelAnimator animator(&leds, NEO_CENTISECONDS);

String client_id;
/*
int last_button_state;
int button_down;
unsigned int hold_start=0;
const unsigned int short_press=120;
const unsigned int medium_press=300;
*/
ButtonControl button;
int last_encoder_position;

int current_preset_index=0;
float current_l_perc=0;
float current_h=0,current_s=0.2,current_l=0;

RotaryEncoder encoder(rotary_pin_left,rotary_pin_right);
enum rotary_mode current_mode=mode_h;

WiFiClient wclient;

void ensure_connected_to_wifi()
{
  if(WiFi.status() != WL_CONNECTED)
  {
    const char* ssid;
    const char* password;

    byte bit0=digitalRead(wifi_bit0);
    byte bit1=digitalRead(wifi_bit1);
    int wifi_index=(bit1<<1)+bit0;
    ssid=wifi_config[wifi_index].ssid;
    password=wifi_config[wifi_index].password;
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ");
    Serial.print(ssid);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
    Serial.print("Connected to WiFi. ");
    Serial.println(WiFi.localIP());
    ArduinoOTA.begin();
  }
}

void encoder_isr()
{
  encoder.tick();
}

AnimUpdateCallback wrap_anim_callback(const AnimUpdateCallback& normal_update, const std::function<void()>& final_update)
{
  AnimUpdateCallback res=[=](float progress)
  {

    if(progress!=1.0)
    {
      normal_update(progress);
    }
    else
    {
      final_update();
    }
  };
  return res;
}

void start_animation_hscroll(float s, float l)
{
  for (uint16_t pixel_index=0;pixel_index<led_count;pixel_index++)
  {
    float h=pixel_index/(float)(led_count+1);
    float h_new=h+1;
    HslColor color_start(h,s,l);
    bool wrapped=false;
    AnimUpdateCallback cb_update=[=](float progress) mutable
    {
      // progress will start at 0.0 and end at 1.0
      float h_new=color_start.H+progress;
      if(h_new>1.0 || wrapped)
      {
        wrapped=true;
        h_new-=1.0;
      }
      HslColor color_new(h_new, color_start.S, color_start.L);
      leds.SetPixelColor(pixel_index, color_new);
    };
    animator.StartAnimation(pixel_index,40/*centiseconds*/,wrap_anim_callback(cb_update,[=]()
    {
      leds.SetPixelColor(pixel_index, HslColor(current_h,current_s,current_l));
    }));
  }
}

void start_animation_lbounce()
{
  for (uint16_t pixel_index=0;pixel_index<led_count;pixel_index++)
  {
    //int factor=(pixel_index%2)?1:-1;
    HslColor color_start(leds.GetPixelColor(pixel_index));
    int factor=(color_start.L>0.5)?-1:1;
    float l_new=color_start.L+factor*0.1;
    HslColor color_stop(color_start.H, color_start.S, l_new);
    AnimUpdateCallback cb_update=[=](float progress) mutable
    {
      // progress will start at 0.0 and end at 1.0
      HslColor color_new;
      if(progress<0.5)
      {
        color_new=HslColor::LinearBlend(color_start, color_stop, 2*progress);
      }
      else
      {
        color_new=HslColor::LinearBlend(color_stop, color_start, 2*(progress-0.5));
      }

      leds.SetPixelColor(pixel_index, color_new);
    };
    animator.StartAnimation(pixel_index,25/*centiseconds*/,wrap_anim_callback(cb_update,[=]()
    {
      leds.SetPixelColor(pixel_index, HslColor(current_h,current_s,current_l));
    }));
  }
}

void setup_ota_update(const String& desc="")
{
    String ota_hostname="esp8266-"+String(ESP.getChipId(), HEX);
    if(desc!="")
        ota_hostname+=" "+desc;
    ArduinoOTA.setHostname(ota_hostname.c_str());
    ArduinoOTA.setPassword("123@Test");

    ArduinoOTA.onStart([]()
    {
        Serial.println("[OTA] Start");
        leds.ClearTo(0,0,0);
        leds.SetPixelColor(0,32,32,0);
        leds.Show();
    });

    ArduinoOTA.onEnd([]()
    {
        Serial.println("\n[OTA] End");
        leds.ClearTo(0,0,0);
        leds.SetPixelColor(0,0,32,0);
        leds.Show();
    });

    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
    {
        Serial.printf("[OTA] Progress: %u%%\r", (progress / (total / 100)));
        float p=(float)progress/(float)total;
        float led_p=1/(float)led_count;
        for(int led_index=0;led_index<led_count;led_index++)
        {
        if(p>=led_index*led_p)
        leds.SetPixelColor(led_index,32,32,32);
        else
        leds.SetPixelColor(led_index,0,0,0);
        }
        leds.Show();
    });

    ArduinoOTA.onError([](ota_error_t error)
    {
        leds.ClearTo(0,0,0);
        leds.SetPixelColor(0,32,0,0);
        leds.Show();

        Serial.printf("[OTA] Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
}

void hsl_wrap(float& v)
{
  if(v>1)
  {
    v-=1;
  }
  else if(v<0)
  {
    v+=1;
  }
}

void clamp(float min, float& v, float max)
{
  if(v<min)
  {
    v=min;
  }
  else if(v>max)
  {
    v=max;
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

    // Rotary Encoder Pins
    pinMode(rotary_pin_left, INPUT_PULLUP);
    pinMode(rotary_pin_right, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(rotary_pin_left),encoder_isr,CHANGE);
    attachInterrupt(digitalPinToInterrupt(rotary_pin_right),encoder_isr,CHANGE);

    // Encoder Button and onboard "User Key"
    /*
    pinMode(button_pin, INPUT_PULLUP);
    last_button_state=digitalRead(button_pin);
    */
    button.begin(button_pin);
    button.onShortClick([]{Serial.println("Short Click");});
    button.onMediumClick([]{Serial.println("Medium Click");});
    button.onLongClick([]{Serial.println("Long Click");});
    button.onDoubleShortClick([]{Serial.println("Double Short Click");});    

    // WiFi Config selection pins
    pinMode(wifi_bit0, INPUT_PULLUP);
    pinMode(wifi_bit1, INPUT_PULLUP);

    // Initialize LEDs
    leds.Begin();
    //leds.ClearTo(0,0,0);
    leds.ClearTo(HslColor(current_h,current_s,current_l));
    //eds.ClearTo(presets[current_preset_index]);
    leds.Show();

    setup_ota_update("led-strip");
}

void loop() {
    ensure_connected_to_wifi();
    ArduinoOTA.handle();

    //int button=digitalRead(button_pin);
    int encoder_position=encoder.getPosition();
    int encoder_delta=last_encoder_position-encoder_position;
    last_encoder_position=encoder_position;

    if(encoder_delta!=0)
    {
        //Serial.print("Encoder delta: ");
        //Serial.println(encoder_delta);

        if(current_mode==mode_h)
        {
            current_h+=encoder_delta*0.01;
            hsl_wrap(current_h);
            Serial.print("h: ");
            Serial.println(current_h);
        }
        else if(current_mode==mode_s)
        {
            current_s+=encoder_delta*0.01;
            clamp(0,current_s,1.0);
            Serial.print("s: ");
            Serial.println(current_s);
        }
        else if(current_mode==mode_l)
        {
            current_l_perc+=encoder_delta;
            clamp(0,current_l_perc,100);
            current_l=current_l_perc*current_l_perc;
            
            Serial.print("l: ");
            Serial.println(current_l);
        }
        leds.ClearTo(HslColor(current_h,current_s,current_l));
        leds.Show();
    }

    button.update();
    
    /*
    if(button!=last_button_state)
    {
        // Button just became pressed
        if(last_button_state==1 && button==0)
        {
            hold_start=millis();
            button_down=1;
        }
        // Button released
        else if(last_button_state==0 && button==1)
        {
            unsigned long hold_time=millis()-hold_start;
            button_down=0;
            if(hold_time>=short_press && hold_time<=medium_press)
            {
                if(current_mode==mode_h)
                {
                    current_mode=mode_s;
                    Serial.println("Mode: s");
                }
                else if(current_mode==mode_s)
                {
                    current_mode=mode_l;
                    start_animation_lbounce();
                    Serial.println("Mode: l");
                }
                else if(current_mode==mode_l)
                {
                    current_mode=mode_h;
                    start_animation_hscroll(0.7,current_l);
                    Serial.println("Mode: h");
                }
                
                // current_preset_index++;
                // if(current_preset_index>=ArrayCount(presets))
                    // current_preset_index=0;
                // leds.ClearTo(presets[current_preset_index]);
                // leds.Show();
            }
            // Medium Press - Turn off LEDs
            else if(hold_time>=medium_press)
            {
                leds.ClearTo(0,0,0);
                leds.Show();
            }
        }
        
        last_button_state=button;
    }
    */

    while(animator.IsAnimating())
    {
        animator.UpdateAnimations();
        leds.Show();
        //delay(31); // ~30hz change cycle
        delay(16); // ~60hz change cycle
    }
}
