#ifndef WIFI_CONNECTOR_H
#define WIFI_CONNECTOR_H

#include <Arduino.h>
#include <functional>
#include "ticker_component.h"
#include <IPAddress.h>

class wifi_connector: public ticker_component
{
private:
    String ssid, password;
    bool connecting;
    bool connected;
public:
    typedef std::function<void()> callback;
    
    wifi_connector();
    void begin(const String& ssid, const String& password);
    void update();
    
    bool is_connected() const;
    
    callback on_connected;
    callback on_disconnected;

    bool host_by_name(String hostname, IPAddress &ip);
};

#endif // WIFI_CONNECTOR_H
