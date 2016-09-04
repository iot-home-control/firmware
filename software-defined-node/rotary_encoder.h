#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include "RotaryEncoder.h"
#include "trampoline_magic.h"
#include "ticker_component.h"

class rotary_encoder: public ticker_component
{
private:
    RotaryEncoder *encoder;
    int isr_tramp_index1,isr_tramp_index2;
public:
    rotary_encoder();

    void begin(unsigned char pin1, unsigned char pin2);
    int get_position();
    void update();
};

#endif // ROTARY_ENCODER_H
