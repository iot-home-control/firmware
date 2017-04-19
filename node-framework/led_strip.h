#pragma once

#include "config.h"
#include "NeoPixelBus.h"
#include "NeoPixelAnimator.h"
#include "ticker_component.h"
#include <memory>
#include <vector>

#if defined(LEDS_RGB)
    #define LedFeatureType NeoGbrFeature
    #define LedColorType RgbColor
    #define LedOffColor RgbColor(0,0,0)
#elif defined(LEDS_RGBW)
    #define LedFeatureType NeoGrbwFeature
    #define LedColorType RgbwColor
    #define LedOffColor RgbwColor(0,0,0,0)
#else
    #error "No LED type"
#endif

typedef NeoPixelBus<LedFeatureType, NeoEsp8266Uart800KbpsMethod> led_type;

class led_strip: public ticker_component
{
private:
    NeoPixelAnimator animator;
    NeoGamma<NeoGammaTableMethod> gamma;
    size_t strip_length;
    bool _is_on=false;
    std::vector<HslColor> hslStrip;

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
