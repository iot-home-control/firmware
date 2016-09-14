#include "led_strip.h"

led_strip::led_strip(size_t length): leds(length, 3), animator(length, NEO_MILLISECONDS), strip_length(length)
{
}

void led_strip::begin()
{
    leds.Begin();
    leds.ClearTo(RgbColor(0,0,0));
    leds.Show();
    _is_on=false;
}

void led_strip::update()
{
    if(animator.IsAnimating())
    {
        animator.UpdateAnimations();
        leds.Show();
    }
}

bool led_strip::is_on() const
{
    return _is_on;
}

void led_strip::rainbow(size_t ms)
{
    if(!is_on())
        return;

    for (uint16_t pixel_index=0;pixel_index<strip_length;pixel_index++)
    {
        float s=1.0, l=0.01;
        float h=pixel_index/(float)(strip_length+1);
        HslColor color_start(h,s,l);
        bool wrapped=false;
        AnimUpdateCallback cb_update=[=](AnimationParam param) mutable
        {
            // progress will start at 0.0 and end at 1.0
            float h_new=color_start.H+param.progress;
            if(h_new>1.0 || wrapped)
            {
                wrapped=true;
                h_new-=1.0;
            }
            HslColor color_new(h_new, color_start.S, color_start.L);
            leds.SetPixelColor(pixel_index, color_new);
        };
        animator.StartAnimation(pixel_index,ms,cb_update);
    }
}

void led_strip::fade_to_color(const HslColor &color, size_t fade_duration_ms)
{
    for (uint16_t pixel_index=0;pixel_index<strip_length;pixel_index++)
    {
        HslColor color_start=leds.GetPixelColor(pixel_index);
        AnimUpdateCallback cb_update=[=](AnimationParam param) mutable
        {
            // progress will start at 0.0 and end at 1.0
            auto set_color = //gamma.Correct(
                        RgbColor(
                            HslColor::LinearBlend<NeoHueBlendShortestDistance>(
                                color_start,
                                color,
                                param.progress)
                            //)
                        );
            leds.SetPixelColor(pixel_index, set_color);
        };
        animator.StartAnimation(pixel_index,fade_duration_ms,cb_update);
    }
}

void led_strip::set_color_to(const HslColor &color)
{
    leds.ClearTo(color);
    leds.Show();
}

void led_strip::turn_on(const HslColor &color, size_t fade_duration_ms)
{
    _is_on=true;
    if(fade_duration_ms!=0)
    {
        HslColor off_color=color;
        off_color.L=0;
        leds.ClearTo(off_color);
        leds.Show();
        fade_to_color(color, fade_duration_ms);
    }
    else
    {
        leds.ClearTo(color);
        leds.Show();
    }
}

void led_strip::turn_off(size_t fade_duration_ms)
{
    _is_on=false;
    for (uint16_t pixel_index=0;pixel_index<strip_length;pixel_index++)
    {
        HslColor color_start=leds.GetPixelColor(pixel_index);
        HslColor color_stop=color_start;
        color_stop.L=0;
        AnimUpdateCallback cb_update=[=](AnimationParam param)
        {
            // progress will start at 0.0 and end at 1.0
            auto set_color = //gamma.Correct(
                        RgbColor(
                            HslColor::LinearBlend<NeoHueBlendShortestDistance>(
                                color_start,
                                color_stop,
                                param.progress)
                            //)
                        );
            leds.SetPixelColor(pixel_index, set_color);
        };
        animator.StartAnimation(pixel_index,fade_duration_ms,cb_update);
    }
}
