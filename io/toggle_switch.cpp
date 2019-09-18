#include "toggle_switch.h"
#include <Arduino.h>

void toggle_switch::begin(unsigned char pin, gpio_pin::pin_dir dir)
{
    _pin.begin(pin, dir);
    this->pin = pin;
    begin_called = true;
}

void toggle_switch::update()
{
    if(!begin_called)
    {
        return;
    }
    _pin.update();
    if(first_update)
    {
        last_state=_pin.read();
        first_update=false;
        return;
    }
    bool state=_pin.read();
    if(state!=last_state)
    {
        last_state=state;
        if(cb)
        {
            cb(state);
        }
    }

}

void toggle_switch::on_state_change(toggle_switch::callback cb)
{
    this->cb=cb;
}

bool toggle_switch::get_state()
{
    return last_state;
}
