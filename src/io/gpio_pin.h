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

#ifndef GPIO_PIN_H
#define GPIO_PIN_H

#include <Arduino.h>
#include <functional>
#include "../components/ticker_component.h"

class gpio_pin: public ticker_component
{
private:
    unsigned char pin;
    bool is_input, is_output;
    bool last_state;
    volatile bool int_state;
    bool begin_called;
public:
    enum pin_dir
    {
        pin_in=INPUT,
        pin_in_pullup=INPUT_PULLUP,
        pin_out=OUTPUT,        
    };
    
    typedef std::function<void(bool)> callback;
    
    gpio_pin();
    void begin(unsigned char pin, pin_dir dir);
    bool read();
    void write(bool value);
    void update();
    
    callback on_changed;
};

#endif // GPIO_PIN_H
