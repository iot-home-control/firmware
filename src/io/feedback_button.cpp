#include "feedback_button.h"
#include <Arduino.h>

void feedback_button::isr()
{
    static bool in_isr=false;
    if(in_isr)
        return;
    in_isr=true;
    bool button=digitalRead(pin);
    unsigned long time_now=millis();

    // Handle double click timeout
    if(first_click_detected // There is a click pending
        && !button_down  // Not currently processing a click
        && (time_now-last_click_stop)>timeout_double_click) // Enough time has passed since the last click
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

feedback_button::feedback_button()
{

}

void feedback_button::begin(unsigned char pin, gpio_pin::pin_dir dir)
{
    _pin.begin(pin, dir);
    this->pin=pin;
    last_button_state=_pin.read();

    isr_index=allocate_trampoline(std::bind(&feedback_button::isr, this));
    attachInterrupt(digitalPinToInterrupt(pin), tramps[isr_index], CHANGE);
}


void feedback_button::update()
{
    isr();

    unsigned long hold_time = millis() - hold_start;


    if(button_down)
    {
        //Serial.print(".");
        /*if(hold_time > click_minimum && hold_time<medium_click) {
            if(!short_reached) {
                Serial.println("Short reached");
                short_reached = true;
            }
        }*/
        if(hold_time >= medium_click && hold_time < long_click ) {
            if(!medium_reached && cb_medium_reached) {
                //Serial.println("Medium reached");
                cb_medium_reached();
                medium_reached = true;
            }

        }
        if(hold_time > long_click) {
            if(!long_reached && cb_long_reached) {
                //Serial.println("Long reached");
                cb_long_reached();
                long_reached = true;
            }
        }
    }
    else {
        short_reached = false;
        medium_reached = false;
        long_reached = false;
        if(cb_released)
        {
            cb_released();
        }
    }



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

void feedback_button::on_short_click(feedback_button::callback cb)
{
    cb_short=cb;
}

void feedback_button::on_medium_click(feedback_button::callback cb)
{
    cb_medium=cb;
}

void feedback_button::on_long_click(feedback_button::callback cb)
{
    cb_long=cb;
}

void feedback_button::on_double_short_click(feedback_button::callback cb)
{
    cb_double_short=cb;
}

void feedback_button::on_long_click_reached(feedback_button::callback cb)
{
    cb_long_reached=cb;
}

void feedback_button::on_medium_click_reached(feedback_button::callback cb)
{
    cb_medium_reached=cb;
}

void feedback_button::on_released(feedback_button::callback cb)
{
    cb_released=cb;
}

