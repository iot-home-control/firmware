// SPDX-License-Identifier: GPL-3.0-or-later
/*
    Copyright (C) 2021  The Home Control Authors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "gpio_pin.h"

gpio_pin::gpio_pin(): last_state(false), begin_called(false)
{
}

void gpio_pin::begin(unsigned char pin, pin_dir dir)
{
    this->pin=pin;
    pinMode(pin,dir);
    is_output=(dir==pin_out);
    is_input=!is_output;
    if(is_input)
        last_state=digitalRead(pin);

    /*int isr_index=allocate_trampoline([this]
    {
        int_state=digitalRead(this->pin);
    });
    attachInterrupt(digitalPinToInterrupt(pin),tramps[isr_index],CHANGE);*/
    begin_called = true;
}

bool gpio_pin::read()
{
    if(!begin_called)
        return false;
    if(is_output)
        return false;
    return last_state;
}

void gpio_pin::write(bool value)
{
    if(!begin_called)
        return;
    if(is_output)
    {
        digitalWrite(pin, value);
    }
}

void gpio_pin::update()
{
    if(!begin_called)
        return;
    if(is_input)
    {
        bool state=digitalRead(pin);
        if(/*int_*/state!=last_state)
        {
            last_state=/*int_*/state;
            if(on_changed)
                on_changed(last_state);
        }
    }
}
