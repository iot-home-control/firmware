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

#ifndef BUTTON_H
#define BUTTON_H

#include <functional>
#include "../components/ticker_component.h"
#include "gpio_pin.h"
#include "../components/trampoline_magic.h"

class feedback_button: public ticker_component
{
public:
    typedef std::function<void(void)> callback;
private:
    enum class event_trigger: unsigned char
    {
        none,
        single_short,
        single_medium,
        single_long,
        double_short,
    };

    gpio_pin _pin;
    unsigned char pin;
    int isr_index=-1;
    callback cb_short, cb_medium, cb_long, cb_double_short, cb_medium_reached, cb_long_reached, cb_released;

    unsigned long hold_start=0;
    unsigned long last_click_stop=0;
    bool first_click_detected=false;
    bool button_down=false;
    bool last_button_state=false;

    const unsigned long timeout_double_click=500;
    const unsigned long click_minimum=25;
    //const unsigned long short_click=150;
    const unsigned long medium_click=300;
    const unsigned long long_click=800;

    bool short_reached = false;
    bool medium_reached = false;
    bool long_reached = false;

    event_trigger trigger;
    void isr();
public:
    feedback_button();
    virtual ~feedback_button()=default;
    void begin(unsigned char pin, gpio_pin::pin_dir dir);
    void update();
    void on_short_click(callback cb);
    void on_medium_click(callback cb);
    void on_long_click(callback cb);
    void on_double_short_click(callback cb);
    void on_long_click_reached(callback cb);
    void on_medium_click_reached(callback cb);
    void on_released(callback cb);
};

#endif // BUTTON_H
