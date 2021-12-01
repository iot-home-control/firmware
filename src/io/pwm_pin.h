#ifndef PWM_PIN_H
#define PWM_PIN_H

#include <Arduino.h>
#include <stdint.h>
#include "../components/ticker_component.h"
#include "../components/node_base.h"

class pwm_pin: public ticker_component
{
private:
    node_base *node=nullptr;

    unsigned char pin;
    bool begin_called;
    String type_name;
    void handle_mqtt(char *topic, unsigned char *data, unsigned int length);
public:
    pwm_pin(String type_name);
    void begin(node_base *node, const int vnode_id, unsigned char pin, uint8_t pwm_bits, uint32_t pwm_frequency);
    void write(uint16_t value);
    void update() override;
};

#endif // pwm_PIN_H
