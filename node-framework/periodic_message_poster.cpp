#include "periodic_message_poster.h"

void periodic_message_poster::begin(mqtt_handler* mqtt, const String& topic, const String& message, unsigned long interval)
{
    this->topic = topic;
    this->message = message;
    this->interval = interval;
    this->last = millis();
    this->mqtt = mqtt;
}

void periodic_message_poster::update() 
{
    unsigned long now = millis();
    if (now - last > interval)
    {
        mqtt->publish(topic,message);
        last = now;
    }
}

void periodic_message_poster::post_now()
{
    mqtt->publish(topic,message);
    last = millis();
}
