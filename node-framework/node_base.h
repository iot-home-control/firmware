#pragma once

#include <Arduino.h>

#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266mDNS.h>

#include "config.h"
#include "wifi_connector.h"
#include "updater_ota.h"
#include "mqtt_handler.h"
#include "periodic_message_poster.h"

#if !defined(HAS_CONFIG_H)
#error "config.h not found!"
#endif

class node_base
{
public:
    node_base(bool serial_alternate_rx_tx=false);
    virtual ~node_base();
    virtual void setup();
    virtual void loop();
    void wait_for_loop_timing();
    String get_state_topic(const String& type);
    String get_action_topic(const String& type);

protected:
    String device_id;
    wifi_connector wifi;
    updater_ota ota;
    mqtt_handler mqtt;
    const float ticks_per_second=30.0f;
    unsigned long cycle_time_ms=(int)(1000/ticks_per_second);
    std::vector<ticker_component*> components;

    virtual void on_wifi_connected();
    virtual void on_wifi_disconnected();

    virtual void on_mqtt_connected();
    virtual void on_mqtt_disconnected();

private:
    unsigned long loop_start_ms;
    periodic_message_poster message_poster;
    bool serial_alternate_pins;
};
