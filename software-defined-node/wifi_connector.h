#ifndef WIFI_CONNECTOR_H
#define WIFI_CONNECTOR_H

#include <Arduino.h>
#include <functional>

class wifi_connector
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
};

#endif // WIFI_CONNECTOR_H
