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
