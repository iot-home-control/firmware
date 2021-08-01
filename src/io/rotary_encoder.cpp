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

    /*isr_tramp_index1 = allocate_trampoline([=]
    {
        encoder->tick();
    });
    isr_tramp_index2 = allocate_trampoline([=]
    {
        encoder->tick();
    });*/

    auto isr = [=]{ encoder->tick(); };
    attachInterrupt(digitalPinToInterrupt(pin1), isr, CHANGE);
    attachInterrupt(digitalPinToInterrupt(pin2), isr, CHANGE);
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

