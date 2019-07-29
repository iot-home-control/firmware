#ifndef SENSOR_DHT22_H
#define SENSOR_DHT22_H

#include "../components/ticker_component.h"
#include <DHT.h>
#include <functional>

class sensor_dht22: public ticker_component
{
private:
    class wrapper
    {
    public:
        DHT dht;
        wrapper(unsigned char pin): dht(pin,DHT22){}
    };
    
    wrapper *sensor;
    unsigned long prev_millis, update_every_ms;
    
    float last_temperature, last_humidity;
    bool always_notify;
public:
    typedef std::function<void(const float)> callback;
    
    sensor_dht22();
    void begin(unsigned char pin, unsigned long update_every_ms, const bool always_notify=false);
    void update() override;
    float get_temperature();
    float get_humidity();
    
    callback on_temperature_changed;
    callback on_humidity_changed;
};

#endif // SENSOR_DHT22_H
