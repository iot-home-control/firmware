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

#include "components/node_base.h"

#include <memory>
#include <functional>

#include "io/gpio_pin.h"
#include "io/feedback_button.h"

class node_switch: public node_base
{
public:
    node_switch();

    void setup();
    void loop();
private:
    feedback_button btn;

    gpio_pin pin_relay;
    gpio_pin pin_led;
    gpio_pin pin_led2;

    bool state=false;

    void switch_power(bool on);
    void handle_action_message(char* topic, unsigned char* data, unsigned int length);
public:
    void handle_short_button();
    void handle_long_button();
    void handle_long_click_reached();
    void handle_medium_click_reached();
    void handle_released();
    void handle_medium_button();
};

node_switch::node_switch(): node_base()
{

}

void node_switch::setup()
{
    node_base::setup();

    pin_relay.begin(5, gpio_pin::pin_out);
//    pin_relay.begin(12, gpio_pin::pin_out);
    components.push_back(&pin_relay);

    pin_led.begin(2, gpio_pin::pin_out);
//    pin_led.begin(13, gpio_pin::pin_out);
    components.push_back(&pin_led);

    pin_led2.begin(13, gpio_pin::pin_out);
    components.push_back(&pin_led2);

    btn.begin(4, gpio_pin::pin_in_pullup);
//    btn.begin(0, gpio_pin::pin_in);
    components.push_back(&btn);

    auto switch_action_handler = std::bind(&node_switch::handle_action_message, this,
                                           std::placeholders::_1,
                                           std::placeholders::_2,
                                           std::placeholders::_3);
    mqtt.handle_topic("/switch/action", switch_action_handler);
    mqtt.handle_topic(get_action_topic("switch"), switch_action_handler);

    btn.on_short_click(std::bind(&node_switch::handle_short_button, this));
    btn.on_long_click(std::bind(&node_switch::handle_long_button, this));
    btn.on_medium_click(std::bind(&node_switch::handle_medium_button, this));
    btn.on_long_click_reached(std::bind(&node_switch::handle_long_click_reached, this));
    btn.on_medium_click_reached(std::bind(&node_switch::handle_medium_click_reached, this));
    btn.on_released(std::bind(&node_switch::handle_released, this));

    pin_led.write(HIGH);
    pin_led2.write(LOW);
}

void node_switch::loop()
{
    node_base::loop();
    node_base::wait_for_loop_timing();
}

void node_switch::switch_power(bool on)
{
    if(on)
    {
        pin_relay.write(HIGH);
        pin_led.write(LOW);
        state = true;
    }
    else
    {
        pin_relay.write(LOW);
        pin_led.write(HIGH);
        state = false;
    }
}

void node_switch::handle_action_message(char *topic, unsigned char *data, unsigned int length)
{
    (void)topic;
    char s[length+1];
    memcpy(s,data,length);
    s[length]=0;
    String data_str(s);
    String publish_topic=get_state_topic("switch");
    if(data_str=="on")
    {
        Serial.println("[MQTT] Turning on.");
        mqtt.publish(publish_topic,"mqtt,on");
        switch_power(true);
    }
    else if(data_str=="off")
    {
        Serial.println("[MQTT] Turning off.");
        mqtt.publish(publish_topic,"mqtt,off");
        switch_power(false);
    }
}

void node_switch::handle_medium_click_reached() {
    pin_led2.write(HIGH);
}

void node_switch::handle_short_button()
{
    Serial.println("short click");
    if(state)
    {

        mqtt.publish(get_state_topic("switch"),"local,off");
        state=false;

    }
    else
    {
        mqtt.publish(get_state_topic("switch"),"local,on");
        state = true;
    }
}

void node_switch::handle_long_button()
{
    Serial.println("long clicked");


    mqtt.publish(get_state_topic("switch",1),"local,on");
    delay(500);
    mqtt.publish(get_state_topic("switch",1),"local,off");



}

void node_switch::handle_medium_button()
{
    Serial.println("medium clicked");
}

void node_switch::handle_long_click_reached()
{
    pin_led.write(LOW);

}

void node_switch::handle_released()
{

    pin_led.write(HIGH);
    pin_led2.write(LOW);
}


//mqtt.publish("/switch/"+client_id+"/state","local,reboot");

node_switch node;

void setup()
{
    node.setup();
}

void loop()
{
    node.loop();
}
