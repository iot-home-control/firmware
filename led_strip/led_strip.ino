#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <RotaryEncoder.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <RgbColor.h>
#include <HslColor.h>
#include "data.h"

#define led_count 15
//#define led_pin 13 // D4
#define button_pin 0 // D3
#define rotary_pin_left  5 // D1
#define rotary_pin_right 4 // D2
#define wifi_bit0 12 // Pullup
#define wifi_bit1 14 // Pullup: 11=Home, 10=Frefunk, 01=Rikus, 00=Reserved

NeoPixelBus leds(led_count, 0); // pin number not required with dma
NeoPixelAnimator animator(&leds, NEO_CENTISECONDS);

String client_id;

int last_button_state;
int button_down;

int last_encoder_position;

int current_color_index=0;

unsigned int hold_start=0;
const unsigned int short_press=120;
const unsigned int medium_press=300;

float current_h=0,current_s=0.2,current_l=0.1;

RotaryEncoder encoder(rotary_pin_left,rotary_pin_right);
WiFiClient wclient;

#define ArrayCount(arr) (sizeof(arr)/sizeof(0[arr]))

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
  {255,0,0},
  {0,255,0},
  {0,0,255},
  {127,127,127},
  {255,255,255},
};

enum rotary_mode
{
  mode_h,
  mode_s,
  mode_l,
};

enum rotary_mode current_mode=mode_h;
const char* ssid_ff     = "Freifunk";
const char* password_ff = "";
const char* ssid_home     = "1084059";
const char* password_home = "2415872658287010";
const char* ssid_rikus     = "ThatsNoWiFi";
const char* password_rikus = "J2UYpGaNUN2gh7nb";

void ensure_connected_to_wifi()
{
  if(WiFi.status() != WL_CONNECTED)
  {
    const char* ssid;
    const char* password;

    byte bit0=digitalRead(wifi_bit0);
    byte bit1=digitalRead(wifi_bit1);
    int wifi_index=(bit1<<1)+bit0;
    Serial.print("bit0: ");
    Serial.print(bit0);
    Serial.print(" bit1: ");
    Serial.print(bit1);
    Serial.print(" ");
    Serial.println(wifi_index);
    
    if(wifi_index==3)
    {
      ssid=ssid_home;
      password=password_home;
    }
    else if(wifi_index==2)
    {
      ssid=ssid_ff;
      password=password_ff;
    }
    else if(wifi_index==1)
    {
      ssid=ssid_rikus;
      password=password_rikus;
    }
    else
    {
      ssid=0;
      password=0;
    }
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

void setup_ota_update()
{
  ArduinoOTA.onStart([]() {
  Serial.println("[OTA] Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\n[OTA] End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("[OTA] Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("[OTA] Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.setPassword("123@Test");
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

void hsl_clamp(float& v)
{
  if(v<0)
  {
    v=0;
  }
  else if(v>1)
  {
    v=1;
  }
}

void setup() {
  client_id="node-"+String(ESP.getChipId());
  
  Serial.begin(115200);
  Serial.pins(15,13);  
  delay(10);
  Serial.println();

  Serial.print("Client-Id: ");
  Serial.println(client_id);
  
  pinMode(button_pin, INPUT_PULLUP);
  last_button_state=digitalRead(button_pin);

  pinMode(wifi_bit0, INPUT_PULLUP);
  pinMode(wifi_bit1, INPUT_PULLUP);

  pinMode(rotary_pin_left, INPUT_PULLUP);
  pinMode(rotary_pin_right, INPUT_PULLUP);

  current_color_index=0;
  leds.Begin();
  //leds.ClearTo(0,0,0);
  leds.ClearTo(HslColor(current_h,current_s,current_l));
  leds.Show();
  
  attachInterrupt(digitalPinToInterrupt(rotary_pin_left),encoder_isr,CHANGE);
  attachInterrupt(digitalPinToInterrupt(rotary_pin_right),encoder_isr,CHANGE);
  setup_ota_update();
}

void loop() {
  ensure_connected_to_wifi();
  ArduinoOTA.handle();
  
  int button=digitalRead(button_pin);
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
      hsl_clamp(current_s);
      Serial.print("s: ");
      Serial.println(current_s);
    }
    else if(current_mode==mode_l)
    {
      current_l+=encoder_delta*0.01;
      hsl_clamp(current_l);
      Serial.print("l: ");
      Serial.println(current_l);
    }
    leds.ClearTo(HslColor(current_h,current_s,current_l));
    leds.Show();
  }


  if(button!=last_button_state)
  {
    /*Serial.print("Button ");
    Serial.print(last_button_state);
    Serial.print(" -> ");
    Serial.println(button);*/
    
    if(last_button_state==1 && button==0) // Button just became pressed
    {
      hold_start=millis();
      button_down=1;
    }
    else if(last_button_state==0 && button==1) // Button released
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
      }
      else if(hold_time>=medium_press)
      {
        leds.ClearTo(0,0,0);
        leds.Show();
      }
    }
    last_button_state=button;
  }
  
  while(animator.IsAnimating())
  {
    animator.UpdateAnimations();
    leds.Show();
    //delay(31); // ~30hz change cycle
    delay(16); // ~60hz change cycle
  }
}
