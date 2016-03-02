#include "ButtonControl.h"
#include <Arduino.h>

void ButtonControl::begin(int pin)
{
    this->pin=pin;
    pinMode(pin, INPUT_PULLUP);
    last_button_state=digitalRead(pin);
}

void ButtonControl::update()
{
    bool button=digitalRead(pin);
    unsigned long time_now=millis();
    
    // Handle double click timeout
    if(first_click_detected // There is a click pending
        && !button_down  // Not currently processing a click
        && (time_now-last_click_stop)>timeout_double_click) // Enough time has passed since the last click
    {
        first_click_detected=false;
        if(cb_short)
            cb_short();
    }
    
    // Button state changed
    if(button!=last_button_state)
    {
        // Button just became pressed
        if(last_button_state && !button)
        {
            hold_start=millis();
            button_down=true;
        }
        // Button released
        else if(!last_button_state && button)
        {            
            unsigned long hold_time=time_now-hold_start;
            button_down=false;
            // Short Click
            if(hold_time>=short_click && hold_time<=medium_click)
            {
                // Remember first click
                if(!first_click_detected)
                {
                    first_click_detected=true;
                    last_click_stop=time_now;
                }
                // Check if double click
                else
                {
                    unsigned long time_between_clicks=hold_start-last_click_stop;
                    first_click_detected=false;
                    if(time_between_clicks<=timeout_double_click && cb_double_short)
                    {
                        cb_double_short();
                    }
                }
            }
            // Medium Click
            else if(hold_time>=medium_click && hold_time<=long_click)
            {
                first_click_detected=false;
                if(cb_medium) {cb_medium();}
            }
            else if(hold_time>=long_click)
            {
                first_click_detected=false;
                if(cb_long) {cb_long();}
            }
        }        
        last_button_state=button;
    }
}

void ButtonControl::onShortClick(ButtonCallback cb)
{
    cb_short=cb;
}

void ButtonControl::onMediumClick(ButtonCallback cb)
{
    cb_medium=cb;
}

void ButtonControl::onLongClick(ButtonCallback cb)
{
    cb_long=cb;
}

void ButtonControl::onDoubleShortClick(ButtonCallback cb)
{
    cb_double_short=cb;
}