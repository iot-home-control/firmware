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

#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <functional>

#include "RotaryEncoder.h"
#include "../components/trampoline_magic.h"
#include "../components/ticker_component.h"
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
