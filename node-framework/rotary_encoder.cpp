#include "rotary_encoder.h"

rotary_encoder::rotary_encoder(): encoder(nullptr)
{
}

void rotary_encoder::begin(unsigned char pin1, unsigned char pin2)
{
    Serial.println(__PRETTY_FUNCTION__);
    Serial.print("pin1 ");
    Serial.print(pin1);
    Serial.print(" pin2 ");
    Serial.println(pin2);


    if(!encoder)
        encoder=new RotaryEncoder(pin1, pin2);

    // Must be after the encoder object has been created because it sets the pins to INPUT without pullup.
    // Or an external pullup resistor on each pin.
    pinMode(pin1, INPUT_PULLUP);
    pinMode(pin2, INPUT_PULLUP);

    isr_tramp_index1 = allocate_trampoline([=]
    {
        //Serial.print("!1");
        encoder->tick();
    });
    isr_tramp_index2 = allocate_trampoline([=]
    {
        //Serial.print("!2");
        encoder->tick();
    });
    Serial.print("Allocated ISR magic ");
    Serial.print(isr_tramp_index1);
    Serial.print(",");
    Serial.println(isr_tramp_index2);

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
}

