#include "sensor_dht22.h"

sensor_dht22::sensor_dht22(): sensor(nullptr), prev_millis(0), last_temperature(0), last_humidity(0)
{
}

void sensor_dht22::begin(unsigned char pin, unsigned long update_every_ms, const bool always_notify)
{
    this->update_every_ms=update_every_ms;
    this->always_notify=always_notify;
    if(!sensor)
        sensor=new wrapper(pin);
    sensor->dht.begin();
}

void sensor_dht22::update()
{
    unsigned long curr_millis=millis();
    if(curr_millis-prev_millis>=update_every_ms)
    {
        prev_millis=curr_millis;
        if(!sensor)
            return;
        
        float temp=sensor->dht.readTemperature();
        float humi=sensor->dht.readHumidity();

        if(!isnan(temp) && (temp!=last_temperature || always_notify))
        {
            last_temperature=temp;
            if(on_temperature_changed)
                on_temperature_changed(temp);
        }
        
        if(!isnan(humi) && (humi!=last_humidity || always_notify))
        {
            last_humidity=humi;
            if(on_humidity_changed)
                on_humidity_changed(humi);
        }
    }
}

float sensor_dht22::get_temperature()
{
    return last_temperature;
}

float sensor_dht22::get_humidity()
{
    return last_humidity;
}
