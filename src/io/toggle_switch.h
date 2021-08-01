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

#pragma once

#include <functional>
#include "../components/ticker_component.h"
#include "gpio_pin.h"

class toggle_switch: public ticker_component
{
public:
    typedef std::function<void(bool)> callback;
private:
    gpio_pin _pin;
    unsigned char pin;
    bool last_state;
    bool first_update=true;
    callback cb;
    bool begin_called=false;

public:
    toggle_switch()=default;
    virtual ~toggle_switch()=default;
    void begin(unsigned char pin, gpio_pin::pin_dir dir);
    void update();
    void on_state_change(callback cb);
    bool get_state();

};
