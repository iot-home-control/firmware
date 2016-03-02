#pragma once

#include <functional>

typedef std::function<void(void)> ButtonCallback;

class ButtonControl
{
private:
    int pin;
    ButtonCallback cb_short, cb_medium, cb_long, cb_double_short;
    unsigned long hold_start=0;
    unsigned long last_click_stop=0;
    bool first_click_detected=false;
    bool button_down=false;
    bool last_button_state=false;
    
    const unsigned long timeout_double_click=200;
    const unsigned long short_click=150;
    const unsigned long medium_click=300;
    const unsigned long long_click=450;
public:
    void begin(int pin);
    void update();
    void onShortClick(ButtonCallback cb);
    void onMediumClick(ButtonCallback cb);
    void onLongClick(ButtonCallback cb);
    void onDoubleShortClick(ButtonCallback cb);
};