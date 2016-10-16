#pragma once

#include "NeoPixelBus.h"
#include "NeoPixelAnimator.h"
#include "ticker_component.h"
#include <memory>

enum class animation_request
{
    nothing,
    repeat_animation,
};

typedef NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> led_type;
/*
class animation
{
public:
    virtual ~animation() {};
    virtual void setup(NeoPixelAnimator &animator, size_t strip_length, led_type& leds)=0;
    virtual animation_request done() { return animation_request::nothing; }
};

class color_fade_animation: public animation
{
private:
    HslColor from, to;
    size_t duration_ms;
public:
    color_fade_animation(HslColor from, HslColor to, size_t duration_ms=500);
    void setup(NeoPixelAnimator &animator, size_t strip_length, led_type& leds) override;
};

class rainbow_animation: public animation
{
private:
    size_t duration_ms;
public:
    rainbow_animation(size_t duration_ms);
    void setup(NeoPixelAnimator &animator, size_t strip_length, led_type& leds) override;
    animation_request done() override;
};

class test_animation: public animation
{
public:
    void setup(NeoPixelAnimator &animator, size_t strip_length, led_type &leds) override;
};
*/

enum class animation_type
{
    none,
    rainbow,
};

class led_strip: public ticker_component
{
private:
    NeoPixelAnimator animator;
    NeoGamma<NeoGammaTableMethod> gamma;
    size_t strip_length;
    bool _is_on=false;

    // Animation stuff
    //animation* current_animation;
    bool animation_playing;
    bool repeat_animation;
    std::function<void(void)> current_animation_setup;

public:
    led_type leds;
    led_strip(size_t length);

    void begin();
    void update() override;

    bool is_on() const;

    void rainbow(size_t ms);
    void fade_to_color(const HslColor& color, size_t fade_duration_ms);
    void set_color_to(const HslColor& color);
    void turn_on(const HslColor& color, size_t fade_duration_ms=0);
    void turn_off(size_t fade_duration_ms=0);

    void play_animation_rainbow(uint16_t duration_ms, bool repeat=true);
    void play_animation_sunrise(uint16_t duration_ms);
    void play_animation_warp_core(HslColor color, uint16_t duration_ms, bool repeat=true);

    void stop_animation();
    bool is_playing_animation();


};
