#include "button.h"
#include <Arduino.h>

void button::isr()
{
    static bool in_isr=false;
    if(in_isr)
        return;
    in_isr=true;
    bool button=digitalRead(pin);
    unsigned long time_now=millis();

    const bool is_double_click_timeout = (time_now-last_click_stop)>timeout_double_click; // Enough time has passed since the last click
    // Handle double click timeout
    if(first_click_detected // There is a click pending
        && !button_down  // Not currently processing a click
        && is_double_click_timeout)
    {
        first_click_detected=false;
        //Serial.println("Short");
        trigger=event_trigger::single_short;
    }

    // Button state changed
    if(button!=last_button_state)
    {
        // Button just became pressed
        if(last_button_state && !button)
        {
            //Serial.println("Button pressed");
            hold_start=millis();
            button_down=true;
        }
        // Button released
        else if(!last_button_state && button)
        {
            unsigned long hold_time=time_now-hold_start;
            //Serial.print("Held for ");
            //Serial.println(hold_time);
            button_down=false;
            // Short Click
            if(hold_time>=click_minimum && hold_time<=medium_click)
            {
                if(cb_double_short)
                {
                    // Remember first click
                    if(!first_click_detected)
                    {
                        //Serial.println("First click detected");
                        first_click_detected=true;
                        last_click_stop=time_now;
                    }
                    // Check if double click
                    else
                    {
                        //Serial.print("Potential double ");
                        unsigned long time_between_clicks=hold_start-last_click_stop;
                        //Serial.println(time_between_clicks);
                        first_click_detected=false;
                        if(time_between_clicks<=timeout_double_click)
                        {
                            //Serial.println("Short double");
                            trigger=event_trigger::double_short;
                        }
                    }
                }
                else
                {
                    first_click_detected=false;
                    trigger=event_trigger::single_short;
                }
            }
            // Medium Click
            else if(hold_time>=medium_click && hold_time<=long_click)
            {
                first_click_detected=false;
                //Serial.println("Medium");
                if(cb_medium)
                    trigger=event_trigger::single_medium;
                else if(cb_short)
                    trigger=event_trigger::single_short;
            }
            // Long Click
            else if(hold_time>=long_click)
            {
                first_click_detected=false;
                if(cb_long)
                {
                //Serial.println("Long");
                trigger=event_trigger::single_long;
                }
                else if(cb_medium)
                {
                    trigger=event_trigger::single_medium;
                }
                else if(cb_short)
                {
                    trigger=event_trigger::single_short;
                }
            }
        }
        last_button_state=button;
    }
    in_isr=false;
}

button::button(): begin_called(false)
{

}

void button::begin(unsigned char pin, gpio_pin::pin_dir dir)
{
    _pin.begin(pin, dir);
    this->pin=pin;
    last_button_state=_pin.read();

    //isr_index=allocate_trampoline(std::bind(&button::isr, this));
    attachInterrupt(digitalPinToInterrupt(pin), [this](){isr();}, CHANGE);
    begin_called = true;
}

void button::update()
{
    if(!begin_called)
        return;
    isr();
    if(trigger==event_trigger::single_short && cb_short)
        cb_short();
    else if(trigger==event_trigger::single_medium && cb_medium)
        cb_medium();
    else if(trigger==event_trigger::single_long && cb_long)
        cb_long();
    else if(trigger==event_trigger::double_short && cb_double_short)
        cb_double_short();

    trigger=event_trigger::none;
}

void button::on_short_click(button::callback cb)
{
    cb_short=cb;
}

void button::on_medium_click(button::callback cb)
{
    cb_medium=cb;
}

void button::on_long_click(button::callback cb)
{
    cb_long=cb;
}

void button::on_double_short_click(button::callback cb)
{
    cb_double_short=cb;
}
