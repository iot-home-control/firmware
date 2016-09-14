#ifndef BUTTON_H
#define BUTTON_H

#include <functional>
#include "ticker_component.h"
#include "gpio_pin.h"
#include "trampoline_magic.h"

class button: public ticker_component
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
    callback cb_short, cb_medium, cb_long, cb_double_short;

    unsigned long hold_start=0;
    unsigned long last_click_stop=0;    
    bool first_click_detected=false;
    bool button_down=false;
    bool last_button_state=false;

    const unsigned long timeout_double_click=500;
    const unsigned long click_minimum=25;
    //const unsigned long short_click=150;
    const unsigned long medium_click=250;
    const unsigned long long_click=400;

    event_trigger trigger;
    void isr();
public:
    button();
    virtual ~button()=default;
    void begin(unsigned char pin, gpio_pin::pin_dir dir);
    void update();
    void on_short_click(callback cb);
    void on_medium_click(callback cb);
    void on_long_click(callback cb);
    void on_double_short_click(callback cb);
};

#endif // BUTTON_H
