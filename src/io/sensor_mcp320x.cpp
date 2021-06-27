#include "sensor_mcp320x.h"

template <int n>
void sensor_mcp320x<n>::begin(unsigned char pin, uint16_t vref, unsigned char active_channels, unsigned long update_every_ms, const bool always_notify)
{
    this->update_every_ms=update_every_ms;
    this->active_channels=active_channels;
    this->always_notify=always_notify;
    if(!sensor)
        sensor=new wrapper<typename map::channel>(pin, vref);
}

template void sensor_mcp320x<1>::begin(unsigned char, uint16_t, unsigned char, unsigned long, const bool);
template void sensor_mcp320x<2>::begin(unsigned char, uint16_t, unsigned char, unsigned long, const bool);
template void sensor_mcp320x<4>::begin(unsigned char, uint16_t, unsigned char, unsigned long, const bool);
template void sensor_mcp320x<8>::begin(unsigned char, uint16_t, unsigned char, unsigned long, const bool);

template <int n>
void sensor_mcp320x<n>::update()
{
    unsigned long curr_millis=millis();
    if(curr_millis-prev_millis>=update_every_ms)
    {
        prev_millis=curr_millis;
        if(!sensor)
            return;
        
        for(int i = 0; i<active_channels; i++)
        {
            int channel = i;
            if(map::mode_shift)
            {
                channel |= (1<<map::mode_shift);
            }

            uint16_t value = sensor->mcp.read(typename map::channel(channel));

            if(value!=last_value[i] || always_notify)
            {
                last_value[i]=value;
                if(on_value_changed)
                    on_value_changed(i, value, sensor->mcp.toAnalog(value));
            }
        }
    }
}

template void sensor_mcp320x<1>::update();
template void sensor_mcp320x<2>::update();
template void sensor_mcp320x<4>::update();
template void sensor_mcp320x<8>::update();



template <int n>
uint16_t sensor_mcp320x<n>::get_value(const unsigned char idx)
{
    return last_value[idx];
}

template uint16_t sensor_mcp320x<1>::get_value(const unsigned char idx);
template uint16_t sensor_mcp320x<2>::get_value(const unsigned char idx);
template uint16_t sensor_mcp320x<4>::get_value(const unsigned char idx);
template uint16_t sensor_mcp320x<8>::get_value(const unsigned char idx);
