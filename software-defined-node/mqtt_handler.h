#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <functional>
#include <vector>
#include "ticker_component.h"

class mqtt_handler: public ticker_component
{
public:
    typedef std::function<void(char* topic, unsigned char* data, unsigned int len)> topic_handler;
    typedef std::function<void()> connection_state_callback;
private:
    struct handler_pair
    {
        String topic;
        topic_handler handler;
        bool subscribed;
    };
    
    WiFiClient wifi_client;
    PubSubClient client;
    bool connecting, connected;
    unsigned long connection_retry_interval, connecting_start_time;
    unsigned long subscription_check_interval, last_subscription_check;
    String mqtt_id, mqtt_user, mqtt_pass;
    std::vector<handler_pair> handlers;
    bool can_connect=false;

    void mqtt_callback(char* topic, uint8_t* data, unsigned int length);
    void check_subscriptions();

public:
    mqtt_handler();
    void begin(const String& host, const String& client_id, const String& user, const String& pass);
    void update();
    void handle_topic(const String& name, topic_handler handler);
    void publish(const String& topic, const String& msg);

    connection_state_callback on_connected;
    connection_state_callback on_disconnected;
};

#endif // MQTT_HANDLER_H
