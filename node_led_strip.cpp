#include "components/node_base.h"

#include <memory>
#include "io/gpio_pin.h"
#include "io/led_strip.h"
#include "io/rotary_encoder.h"
#include "components/util.h"

static RgbColor colors[]={
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

static HslColor presets_hsl[]={
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


class node_led_strip: public node_base
{
public:
    void setup();
    void loop();
private:
    led_strip leds{LED_STRIP_LENGTH};
    rotary_encoder encoder;
    unsigned int preset_index=0;
    float relative_brightness = 1.0;
    HslColor get_current_color();
    void apply_preset(bool fade=true);
    void handle_action_message(char *topic, unsigned char *data, unsigned int length);
    void handle_color_message(char *topic, unsigned char *data, unsigned int length);
};

HslColor node_led_strip::get_current_color()
{
    HslColor c = presets_hsl[preset_index];
    c.L *= relative_brightness;

    return c;
}

void node_led_strip::apply_preset(bool fade)
{
    if(!leds.is_on())
        return;

    HslColor c = get_current_color();
    if(fade)
        leds.fade_to_color(c, 500);
    else
        leds.set_color_to(c);
}


void node_led_strip::handle_action_message(char *topic, unsigned char *data, unsigned int length)
{
    (void)topic;
    char s[length+1];
    memcpy(s,data,length);
    s[length]=0;
    String data_str(s);
    String publish_topic=get_state_topic("leds");

    if(data_str=="on")
    {
        Serial.println("[MQTT] Turning on.");
        leds.turn_on(get_current_color(),500);
        mqtt.publish(publish_topic,"mqtt,on");
    }
    else if(data_str=="off")
    {
        Serial.println("[MQTT] Turning off.");
        leds.turn_off(500);
        mqtt.publish(publish_topic,"mqtt,off");
    }
    else if(data_str=="sunrise")
    {
        Serial.println("[MQTT] Starting Sunrise");
        leds.play_animation_sunrise(3000,led_strip::anim_duration::anim_long);
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
        leds.play_animation_rainbow(2500,led_strip::anim_duration::anim_short, true);
    }
    else if(data_str=="rainbow_stop")
    {
        Serial.println("[MQTT] Starting rainbow");
        leds.play_animation_rainbow(2500, led_strip::anim_duration::anim_short, false);
    }
    else if(data_str.startsWith("preset="))
    {
        unsigned int preset_num = data_str.substring(7).toInt();
        Serial.print("[MQTT] Setting preset ");
        Serial.println(preset_num);
        preset_index=clamp(0u,preset_num,ARRAY_COUNT(presets_hsl)-1);
        apply_preset();
    }
}

void node_led_strip::handle_color_message(char *topic, unsigned char *data, unsigned int length)
{
    (void)topic;
    char s[length+1];
    memcpy(s,data,length);
    s[length]=0;
    String data_str(s);
    String publish_topic=get_state_topic("leds");
    String the_topic(topic);
    if(the_topic.endsWith("rgb"))
    {
        int rgb[3]={};
        int color=0;
        size_t last_comma=0;
        while(color<3)
        {
            int comma = data_str.indexOf(',', last_comma);
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
}


void node_led_strip::setup() {
    node_base::setup();
    leds.begin();
    components.push_back(&leds);


    auto led_action_handler = std::bind(&node_led_strip::handle_action_message, this,
                                           std::placeholders::_1,
                                           std::placeholders::_2,
                                           std::placeholders::_3);

    auto led_color_handler = std::bind(&node_led_strip::handle_color_message, this,
                                           std::placeholders::_1,
                                           std::placeholders::_2,
                                           std::placeholders::_3);

    mqtt.handle_topic(get_action_topic("leds"), led_action_handler);
    mqtt.handle_topic("/leds/action", led_action_handler);

    mqtt.handle_topic("/leds/color/rgb", led_color_handler);
    mqtt.handle_topic(get_action_topic("leds", 0, "color/rgb"), led_color_handler);

    encoder.begin(5,4,0);
    encoder.on_encoder_changed([&](int delta)
    {
        relative_brightness+=delta*0.01;
        relative_brightness=clamp(0.0f, relative_brightness, 1.0f);
        Serial.print("Brightness: ");
        Serial.println(relative_brightness);
        apply_preset(false);
    });
    encoder.btn.on_short_click([this]
    {
        if(leds.is_playing_animation())
        {
            leds.stop_animation();
            return;
        }
        preset_index=(preset_index+1)%(ARRAY_COUNT(presets_hsl)-1);
        //leds.fade_to_color(presets_hsl[preset_index], 500);
        apply_preset();
        Serial.print("Preset:");
        Serial.println(preset_index);
    });
    encoder.btn.on_medium_click([this]
    {
        static bool state=false;
        if(state)
        {
            Serial.println("Turning off.");
            leds.turn_off(500);
            mqtt.publish(get_state_topic("leds"),"local,off");
        }
        else
        {
            Serial.println("Turning on.");
            leds.turn_on(get_current_color(),500);
            mqtt.publish(get_state_topic("leds"),"local,on");
        }
        state=!state;
    });

    encoder.btn.on_long_click([this]
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
void node_led_strip::loop() {
    node_base::loop();
    node_base::wait_for_loop_timing();
}


node_led_strip node;

void setup()
{
    node.setup();
}

void loop()
{
    node.loop();
}


