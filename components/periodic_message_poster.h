#ifndef PERIODIC_MESSAGE_POSTER_H
#define PERIODIC_MESSAGE_POSTER_H

#include <Arduino.h>
#include "ticker_component.h"
#include "mqtt_handler.h"


class periodic_message_poster: public ticker_component
{
private:
    String topic;
    String message;
    unsigned long interval;
    unsigned long last;
    mqtt_handler *mqtt;

public:
    void update();
    void begin(mqtt_handler* mqtt, const String& topic, const String& message, unsigned long interval);
    void post_now();
};

#endif
