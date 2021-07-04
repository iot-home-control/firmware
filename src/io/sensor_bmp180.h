#pragma once

#include "../components/ticker_component.h"
#include <SFE_BMP180.h>
#include <functional>

class sensor_bmp180: public ticker_component
{
private:
    SFE_BMP180 *sensor;
    unsigned long prev_millis, update_every_ms;

    float last_temperature, last_abs_pressure;
    float altitude;
    bool always_notify;
public:
    typedef std::function<void(const float)> callback;
    typedef std::function<void(const float, const float)> callback2; // absolute, sea-level relative pressure

    sensor_bmp180();
    void begin(unsigned long update_every_ms, const bool always_notify=false, const float altitude=NAN);
    void update() override;
    float get_temperature();
    float get_absolute_pressure();
    float get_relative_pressure();

    callback on_temperature_changed;
    callback2 on_pressure_changed;
};