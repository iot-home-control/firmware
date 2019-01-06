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