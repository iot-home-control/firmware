#include "node_base.h"

#include <memory>
#include <functional>

#include "gpio_pin.h"
#include "button.h"

class node_power_switch: public node_base
{
public:
    node_power_switch();

    void setup();
    void loop();
private:
    button btn;

    gpio_pin pin_relay;
    gpio_pin pin_led;

    bool state=false;

    void switch_power(bool on);
    void handle_action_message(char* topic, unsigned char* data, unsigned int length);
public:
    void handle_button();
};

node_power_switch::node_power_switch(): node_base()
{

}

void node_power_switch::setup()
{
    node_base::setup();

    pin_relay.begin(5, gpio_pin::pin_out);
    components.push_back(&pin_relay);

    pin_led.begin(2, gpio_pin::pin_out);
    components.push_back(&pin_led);

    btn.begin(4, gpio_pin::pin_in);
    components.push_back(&btn);

    auto switch_action_handler = std::bind(&node_power_switch::handle_action_message, this,
                                           std::placeholders::_1,
                                           std::placeholders::_2,
                                           std::placeholders::_3);
    mqtt.handle_topic("/switch/action", switch_action_handler);
    mqtt.handle_topic(get_action_topic("switch"), switch_action_handler);

    btn.on_short_click(std::bind(&node_power_switch::handle_button, this));

    pin_led.write(HIGH);
}

void node_power_switch::loop()
{
    node_base::loop();
    node_base::wait_for_loop_timing();
}

void node_power_switch::switch_power(bool on)
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

void node_power_switch::handle_action_message(char *topic, unsigned char *data, unsigned int length)
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

void node_power_switch::handle_button()
{
    if(state)
    {
        Serial.println("Switch off the relay");
        mqtt.publish(get_state_topic("switch"),"local,off");
        switch_power(false);

    }
    else
    {
        Serial.println("Switch on the relay");
        mqtt.publish(get_state_topic("switch"),"local,on");
        switch_power(true);
    }
}

//mqtt.publish("/switch/"+client_id+"/state","local,reboot");

node_power_switch node;

void setup()
{
    node.setup();
}

void loop()
{
    node.loop();
}
