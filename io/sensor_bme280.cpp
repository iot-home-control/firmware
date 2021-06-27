#include "sensor_bme280.h"

sensor_bme280::sensor_bme280(): sensor(nullptr), prev_millis(0), last_temperature(0), last_abs_pressure(0), altitude(NAN)
{
}

void sensor_bme280::begin(unsigned long update_every_ms, const bool always_notify, const float altitude)
{
    this->update_every_ms = update_every_ms;
    this->always_notify = always_notify;
    this->altitude = altitude;
    if(!sensor)
        sensor = new Adafruit_BME280();
    sensor->begin();
}

void sensor_bme280::update()
{
    unsigned long curr_millis=millis();
    if(curr_millis-prev_millis>=update_every_ms)
    {
        prev_millis=curr_millis;
        if(!sensor)
            return;

        int sensor_delay = sensor->startTemperature();
        if(!sensor_delay)
            return;
        delay(sensor_delay);

        double temperature;
        if(!sensor->getTemperature(temperature))
            return;

        sensor_delay = sensor->startPressure(3);
        if(!sensor_delay)
            return;
        delay(sensor_delay);

        double abs_pressure;
        if(!sensor->getPressure(abs_pressure, temperature))
            return;

        if(temperature != last_temperature || always_notify)
        {
            last_temperature = temperature;
            if(on_temperature_changed)
                on_temperature_changed(temperature);
        }

        if(abs_pressure != last_abs_pressure || always_notify)
        {
            last_abs_pressure = abs_pressure;
            if(on_pressure_changed)
                on_pressure_changed(abs_pressure, get_relative_pressure());
        }
    }
}

float sensor_bme280::get_temperature()
{
    return last_temperature;
}

float sensor_bme280::get_absolute_pressure()
{
    return last_abs_pressure;
}

float sensor_bme280::get_relative_pressure()
{
    if(isnan(altitude))
        return 0.0f;
    return sensor->sealevel(last_abs_pressure, altitude);
}
