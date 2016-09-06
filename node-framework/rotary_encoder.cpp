#include "rotary_encoder.h"

rotary_encoder::rotary_encoder(): encoder(nullptr)
{
}

void rotary_encoder::begin(unsigned char pin1, unsigned char pin2, unsigned char button_pin)
{
    if(!encoder)
        encoder=new RotaryEncoder(pin1, pin2);
    last_position=get_position();

    btn.begin(button_pin, gpio_pin::pin_in_pullup);

    // Must be after the encoder object has been created because it sets the pins to INPUT without pullup.
    // Or an external pullup resistor on each pin.
    pinMode(pin1, INPUT_PULLUP);
    pinMode(pin2, INPUT_PULLUP);

    isr_tramp_index1 = allocate_trampoline([=]
    {
        encoder->tick();
    });
    isr_tramp_index2 = allocate_trampoline([=]
    {
        encoder->tick();
    });

    attachInterrupt(digitalPinToInterrupt(pin1),tramps[isr_tramp_index1], CHANGE);
    attachInterrupt(digitalPinToInterrupt(pin2),tramps[isr_tramp_index2], CHANGE);
}

int rotary_encoder::get_position()
{
    if(encoder)
        return encoder->getPosition();
    return 0;
}

void rotary_encoder::update()
{
    btn.update();

    int current_position=get_position();
    if(current_position!=last_position)
    {
        int diff=current_position-last_position;
        last_position=current_position;
        if(cb_changed)
            cb_changed(diff);
    }
}

void rotary_encoder::on_encoder_changed(rotary_encoder::callback_encoder_changed cb)
{
    cb_changed=cb;
}

