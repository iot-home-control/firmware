#include "pwm_pin.h"

pwm_pin::pwm_pin(String type_name): begin_called(false), type_name(type_name)
{
}

void pwm_pin::begin(node_base *node, const int vnode_id, unsigned char pin, uint8_t pwm_bits, uint32_t pwm_frequency)
{
    this->node = node;
    this->pin=pin;
    pinMode(pin, OUTPUT);
    analogWriteFreq(pwm_frequency);
    analogWriteResolution(pwm_bits);
    begin_called = true;

    node->mqtt.handle_topic(node->get_action_topic(type_name, vnode_id), std::bind(&pwm_pin::handle_mqtt, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void pwm_pin::write(uint16_t value)
{
    if(!begin_called)
        return;
    analogWrite(pin, value);
}

void pwm_pin::update() {}

void pwm_pin::handle_mqtt(char *topic, unsigned char *data, unsigned int length)
{
    (void)topic;
    char s[length+1];
    memcpy(s,data,length);
    s[length]=0;
    String data_str(s);

    //Serial.printf("Setting PWM to %ld\n", data_str.toInt());
    write(data_str.toInt());
}
