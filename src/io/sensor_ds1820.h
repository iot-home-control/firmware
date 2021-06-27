#pragma once

#include "../components/ticker_component.h"

#include <functional>
#include <vector>
#include <DallasTemperature.h>
#include <OneWire.h>

class sensor_ds1820: public ticker_component
{
private:
    struct storable_onewire_address
    {
        DeviceAddress addr{0};
    };
    OneWire *onewire=nullptr;
    DallasTemperature dtlib;
    std::vector<storable_onewire_address> onewire_addresses;
    std::vector<float> last_temperatures;
    unsigned long prev_millis, update_every_ms;
    bool always_notify;
    int vnode_offset;
public:
    typedef std::function<void(const uint8_t /*sensor_index*/, const float /*temperature_in_deg_c*/, const int/*vnode_offset*/)> callback;

    sensor_ds1820();
    void begin(uint8_t pin, const unsigned long update_every_ms, const bool always_notify=false, const int vnode_offset=0);
    void update() override;

    callback on_temperature_changed;
};
