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
