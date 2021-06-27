#pragma once

#include "gpio_pin.h"
#include "button.h"
#include "toggle_switch.h"

class node_base;

class relay: public ticker_component
{
public:
    relay(const String type_name);
    void begin(node_base *node, const int vnode_id, const char relay_pin, const char led_pin=-1, const char button_pin=-1, const bool button_pullup=true, const char toggle_pin=-1, const bool toggle_pullup=true, const bool start_on=false, const bool invert_relay=false, const bool invert_led=false);
    void update();

    void handle_mqtt(char *topic, unsigned char *data, unsigned int length);
    void switch_power(const bool on);
    void switch_state(const bool on, const bool local);
private:
    String type_name;
    node_base *node=nullptr;
    bool start_on;
    bool state;
    bool invert_relay;
    bool invert_led;
    gpio_pin relay_;
    gpio_pin led_;
    button button_;
    toggle_switch toggle_;
};
