#include "gpio_pin.h"

gpio_pin::gpio_pin(): last_state(false)
{
}

void gpio_pin::begin(unsigned char pin, pin_dir dir)
{
    this->pin=pin;
    pinMode(pin,dir);
    is_output=(dir==pin_out);
    is_input=!is_output;
}

bool gpio_pin::read()
{
    if(is_output)
        return false;
    return last_state;
}

void gpio_pin::write(bool value)
{
    if(is_output)
    {
        digitalWrite(pin, value);
    }
}

void gpio_pin::update()
{
    if(is_input)
    {
        bool state=digitalRead(pin);
        if(state!=last_state)
        {
            last_state=state;
            if(on_changed)
                on_changed(last_state);
        }
    }
}
