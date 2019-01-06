#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <functional>

#include "RotaryEncoder.h"
#include "trampoline_magic.h"
#include "ticker_component.h"
#include "button.h"

class rotary_encoder: public ticker_component
{
public:
    typedef std::function<void(int)> callback_encoder_changed;
private:
    RotaryEncoder *encoder;
    int isr_tramp_index1,isr_tramp_index2;
    int last_position=0;
    callback_encoder_changed cb_changed;
public:
    button btn;
    rotary_encoder();

    // left, right, button
    void begin(unsigned char pin1, unsigned char pin2, unsigned char button_pin);
    int get_position();
    void update();

    void on_encoder_changed(callback_encoder_changed cb);
};

#endif // ROTARY_ENCODER_H
