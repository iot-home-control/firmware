#include "relay.h"

#include "node_base.h"

relay::relay(const String type_name): type_name(type_name)
{
}

void relay::begin(node_base *node, const int vnode_id, const char relay_pin, const char led_pin, const char button_pin, const bool start_on, const bool invert_relay, const bool invert_led)
{
    this->node = node;
    this->start_on = start_on;
    this->state = start_on;
    this->invert_relay = invert_relay;
    this->invert_led = invert_led;

    //Serial.printf("begin: vnode_id=%d relay_pin=%d, led_pin=%d, button_pin=%d, start_on=%d, invert_relay=%d, invert_led=%d\n", vnode_id, relay_pin, led_pin, button_pin, start_on, invert_relay, invert_led);

    relay_.begin(relay_pin, gpio_pin::pin_out);
    if(led_pin>=0)
    {
        led_.begin(led_pin, gpio_pin::pin_out);
    }
    if(button_pin>=0)
    {
        button_.begin(button_pin, gpio_pin::pin_in_pullup);
        button_.on_short_click([this]
        {
            switch_state(!state, true);
        });
    }

    node->mqtt.handle_topic(node->get_action_topic(type_name, vnode_id), std::bind(&relay::handle_mqtt, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    switch_power(start_on);
}

void relay::update()
{
    relay_.update();
    led_.update();
    button_.update();
}

void relay::handle_mqtt(char *topic, unsigned char *data, unsigned int length)
{
    (void)topic;
    char s[length+1];
    memcpy(s,data,length);
    s[length]=0;
    String data_str(s);

    const bool on = data_str=="on";
    switch_state(on, false);
}

void relay::switch_power(const bool on)
{
    const bool new_relay_state = (on&&!invert_relay ) || (!on&&invert_relay);
    const bool new_led_state = (on&&!invert_led ) || (!on&&invert_led);

    //Serial.printf("switch_power: on=%d -> relay=%d, led=%d\n", on, new_relay_state, new_led_state);

    relay_.write(new_relay_state?HIGH:LOW);
    led_.write(new_led_state?HIGH:LOW);
    state = on;
}

void relay::switch_state(const bool on, const bool local)
{
    Serial.printf("switch_state: %s,%s\n", local?"local":"mqtt", on?"on":"off");
    switch_power(on);
    node->mqtt.publish(node->get_state_topic(type_name), String(local?"local,":"mqtt,")+String(on?"on":"off"));
};
