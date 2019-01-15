#include "components/node_base.h"

#include <memory>
#include "components/util.h"

#include <NeoPixelBus.h>

struct segment_t
{
    uint8_t pattern;
    uint16_t start;
    uint16_t stop;
    uint16_t speed;
    bool reverse;
    RgbColor colors[3];

    segment_t(): pattern(0), start(0), stop(7), speed(1000), reverse(false), colors{RgbColor(255, 0, 0), RgbColor(0, 0, 0), RgbColor(0, 0, 255)} {}
    uint8_t length() const
    {
        return stop-start+1;
    }
};

struct segment_state_t
{
    uint32_t call_counter;
    uint32_t aux1;
    unsigned long next_call;
    uint16_t aux2;
    bool done;

    void reset()
    {
        call_counter = 0;
        next_call = 0;
        aux1 = 0;
        aux2 = 0;
        done = false;
    }
};

typedef NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart1800KbpsMethod> led_strip;
typedef std::function<uint16_t(led_strip &leds, const segment_t &segment, segment_state_t &state)> PatternFunction;
//typedef uint16_t (*PatternFunction)(led_strip &leds, const segment_t &segment, segment_state_t &state);

RgbColor color_wheel(uint8_t pos)
{
    pos = 255-pos;
    uint32_t ret_hex;
    if(pos < 85)
    {
        ret_hex = ((uint32_t)(255-pos*3)<<16) /*| ((uint32_t)(0)<<8)*/ | (pos*3);
    }
    else if(pos < 170)
    {
        pos -= 85;
        ret_hex = /*((uint32_t)(0) << 16) |*/ ((uint32_t)(pos*3)<<8) | (255-pos*3);
    }
    else
    {
        pos -= 170;
        ret_hex = ((uint32_t)(pos*3)<<16) | ((uint32_t)(255-pos*3)<<8) /*| (0)*/;
    }
    return HtmlColor(ret_hex);
}

uint16_t mode_static(led_strip &leds, const segment_t &segment, segment_state_t &state)
{
    if(state.call_counter == 0)
    {
        for(uint16_t i=segment.start; i <= segment.stop; i++)
        {
            leds.SetPixelColor(i, segment.colors[0]);
        }
    }
    return 1000;
}

uint16_t mode_single_dynamic(led_strip &leds, const segment_t &segment, segment_state_t &state)
{
    if(state.call_counter == 0)
    {
        for(uint16_t i=segment.start; i <= segment.stop; i++)
        {
            leds.SetPixelColor(i, color_wheel(random(256)));
        }
    }
    RgbColor color = color_wheel(random(256));
    const int offset = random(segment.length());
    leds.SetPixelColor(segment.start + offset, color);
    return segment.speed;
}

uint16_t color_wipe(led_strip &leds, const segment_t &segment, segment_state_t &state, RgbColor color1, RgbColor color2, bool rev)
{
    const uint16_t length = segment.length();
    if(state.aux1 < length)
    {
        uint32_t led_offset = state.aux1;
        if(segment.reverse) {
            leds.SetPixelColor(segment.stop - led_offset, color1);
        } else {
            leds.SetPixelColor(segment.start + led_offset, color1);
        }
    }
    else
    {
        uint32_t led_offset = state.aux1 - length;
        if(segment.reverse ^ rev) {
            leds.SetPixelColor(segment.stop - led_offset, color2);
        } else {
            leds.SetPixelColor(segment.start + led_offset, color2);
        }
    }

    state.aux1 = (state.aux1 + 1) % (length * 2);
    return (segment.speed / (length * 2));
}

/*
* Lights all LEDs one after another.
*/
uint16_t mode_color_wipe(led_strip &leds, const segment_t &segment, segment_state_t &state)
{
    return color_wipe(leds, segment, state, segment.colors[0], segment.colors[1], false);
}

void fade_out(led_strip &leds, const segment_t &segment)
{
    for(uint16_t i=segment.start; i<=segment.stop; ++i)
    {
        const RgbColor &cur = leds.GetPixelColor(i);
        RgbColor tmp(cur.R/2, cur.G/2, cur.B/2);
        leds.SetPixelColor(i, tmp);
    }
}

/*
* twinkle_fade function
*/
uint16_t twinkle_fade(led_strip &leds, const segment_t &segment, segment_state_t &state, const RgbColor color) {
    fade_out(leds, segment);

    if(random(3) == 0)
    {
        leds.SetPixelColor(segment.start+random(segment.length()), color);
    }
    return segment.speed/8;
}

/*
* Blink several LEDs on, fading out.
*/
uint16_t mode_twinkle_fade(led_strip &leds, const segment_t &segment, segment_state_t &state)
{
    return twinkle_fade(leds, segment, state, segment.colors[0]);
}

uint16_t mode_twinkle_fade_random(led_strip &leds, const segment_t &segment, segment_state_t &state)
{
    return twinkle_fade(leds, segment, state, color_wheel(random(256)));
}

  /*
  * Firing comets from one end.
  */
uint16_t mode_comet(led_strip &leds, const segment_t &segment, segment_state_t &state)
{
    fade_out(leds, segment);

    if(segment.reverse)
        leds.SetPixelColor(segment.stop - state.aux1, segment.colors[0]);
    else
        leds.SetPixelColor(segment.start + state.aux1, segment.colors[0]);

    const uint16_t length = segment.length();
    state.aux1 = (state.aux1 + 1) % length;
    return (segment.speed / length);
}

/*
* Fire flicker function
*/
uint16_t fire_flicker(led_strip &leds, const segment_t &segment, segment_state_t &state, int rev_intensity)
{
    const RgbColor &col = segment.colors[0];
    const uint8_t lum = max(col.R, max(col.G, col.B)) / rev_intensity;
    for(uint16_t i=segment.start; i <= segment.stop; i++) {
        const uint8_t flicker = random(0, lum);
        leds.SetPixelColor(i, RgbColor(max(col.R - flicker, 0), max(col.G - flicker, 0), max(col.B - flicker, 0)));
    }
    return (segment.speed / segment.length());
}

/*
* Random flickering.
*/
uint16_t mode_fire_flicker(led_strip &leds, const segment_t &segment, segment_state_t &state) {
    return fire_flicker(leds, segment, state, 3);
}

/*
* Random flickering, less intesity.
*/
uint16_t mode_fire_flicker_soft(led_strip &leds, const segment_t &segment, segment_state_t &state) {
    return fire_flicker(leds, segment, state, 6);
}

/*
* Random flickering, more intesity.
*/
uint16_t mode_fire_flicker_intense(led_strip &leds, const segment_t &segment, segment_state_t &state) {
    return fire_flicker(leds, segment, state, 1);
}

uint16_t rainbow(led_strip &leds, const segment_t &segment, segment_state_t &state)
{
    for(uint16_t i=segment.start; i<=segment.stop; ++i)
    {
        leds.SetPixelColor(i, color_wheel((i*256/segment.length()+state.aux1)%256));
    }
    state.aux1 = (state.aux1+1)%256;
    return segment.speed/segment.length();
}

RgbColor sunrise_color(int x)
{
    uint8_t r, g, b;

    if(x<=0)
        return RgbColor(0, 0, 0);
    else if(x>=299)
        return RgbColor(128, 128, 255);

    if(x<29)
        r = 8.785713f*x;
    else if(x<100)
        r = 255;
    else if(x<279)
        r = -0.581006f*(x-100)+254;
    else
        r = -1.05f*(x-279)+149;

    if(x<31)
        g = 0;
    else if(x<99)
        g = 2.164180f*(x-31)+2;
    else if(x<279)
        g = 150;
    else
        g = -1.05f*(x-279)+149;

    if(x<100)
        b = 0;
    else if(x<170)
        b = 1.289854f*(x-100)+1;
    else if(x<290)
        b = 1.277311f*(x-170)+92;
    else
        b = 1.111110f*(x-290)+245;

    return RgbColor(r, g, b);
}

uint16_t mode_sunrise(led_strip &leds, const segment_t &segment, segment_state_t &state)
{
    const uint16_t length=segment.length();
    const bool even = length%2==0;
    const uint16_t side = length/2;
    const uint16_t center1 = side;
    const uint16_t center2 = center1+1;
    const uint16_t steps = 299+side+1;

    if(state.done)
        return 1000;

    if(even)
    {
        for(uint16_t i=segment.start; i<=segment.stop; ++i)
        {
            const uint16_t index = i-segment.start;

            uint16_t dist = 0;
            if(index<=center1)
                dist = center1-index;
            else
                dist = index - center2;

            leds.SetPixelColor(i, sunrise_color(state.aux1-dist));
        }
    }
    else
    {
        for(uint16_t i=segment.start; i<=segment.stop; ++i)
        {
            const uint16_t index = i-segment.start;
            const uint16_t dist = abs(center1-index);

            leds.SetPixelColor(i, sunrise_color(state.aux1-dist));
        }
    }

    if(state.aux1<steps)
        state.aux1++;
    else
        state.done = true;

    const float delay = segment.speed*1000.0f/float(steps);
    return delay;
}

const uint8_t num_segments = 2;

class node_led_test: public node_base
{
public:
    void setup();
    void loop();
private:
    led_strip leds{15};
    segment_t segments[num_segments];
    segment_state_t segment_states[num_segments];
    PatternFunction patterns[9] = {
        &mode_static,
        &mode_single_dynamic,
        &mode_color_wipe,
        &mode_twinkle_fade,
        &mode_twinkle_fade_random,
        &mode_comet,
        &mode_fire_flicker_intense,
        &rainbow,
        &mode_sunrise,
    };
};

void node_led_test::setup()
{
    node_base::setup();
    leds.Begin();
    leds.ClearTo(HtmlColor(0x000000));
    leds.Show();

    for(uint8_t i=0; i<num_segments; ++i)
    {
        segment_states[i].reset();
    }
    segments[0].start=0;
    segments[0].stop=10;
    segments[0].pattern=8;
    segments[0].speed=10;

    segments[1].start=11;
    segments[1].stop=14;
    segments[1].pattern=4;
    segments[1].speed=100;
}

void node_led_test::loop()
{
    node_base::loop();
    bool do_show = false;
    for(uint8_t i=0; i<num_segments; ++i)
    {
        const unsigned long now = millis();
        const segment_t &segment = segments[i];
        segment_state_t &state = segment_states[i];
        if(now>state.next_call)
        {
            uint16_t delay = (patterns[segment.pattern])(leds, segment, state);
            state.call_counter++;
            state.next_call = now+max(delay, (uint16_t)cycle_time_ms);
            do_show = true;
        }
    }
    if(do_show)
        leds.Show();
    node_base::wait_for_loop_timing();
}

node_led_test node;

void setup()
{
    node.setup();
}

void loop()
{
    node.loop();
}


