#pragma once

#include "NeoPixelBus.h"
#include "NeoPixelAnimator.h"
#include "ticker_component.h"

class led_strip: public ticker_component
{
private:
    NeoPixelAnimator animator;
    NeoGamma<NeoGammaTableMethod> gamma;
    size_t strip_length;
public:
    NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> leds;
    led_strip(size_t length);

    void begin();
    void update() override;

    void rainbow(size_t ms);
    void fade_to_color(const HslColor& color, size_t fade_duration_ms);
    void set_color_to(const HslColor& color);
    void turn_on(const HslColor& color, size_t fade_duration_ms=0);
    void turn_off(size_t fade_duration_ms=0);
};
