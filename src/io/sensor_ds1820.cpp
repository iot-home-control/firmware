#include "sensor_ds1820.h"

sensor_ds1820::sensor_ds1820(): prev_millis(0), vnode_offset(0)
{

}

void sensor_ds1820::begin(uint8_t pin, const unsigned long update_every_ms, const bool always_notify, const int vnode_offset)
{
    this->update_every_ms=update_every_ms;
    this->always_notify=always_notify;
    this->vnode_offset=vnode_offset;
    onewire=new OneWire(pin);
    dtlib.setOneWire(onewire);
    dtlib.begin();
    int device_count=dtlib.getDeviceCount();
#if defined(DS1820_DEBUG)
    Serial.print("There are ");
    Serial.print(device_count);
    Serial.println(" OneWire devices on the bus.");
#endif
    last_temperatures.resize(device_count, 0);

    bool need_new_address=true;

    for(uint8_t device_index=0; device_index<device_count; ++device_index)
    {
        if(need_new_address)
            onewire_addresses.emplace_back();
        auto &addr=onewire_addresses.back();
        if(dtlib.getAddress(addr.addr,device_index))
        {
            need_new_address=true;
#if defined(DS1820_DEBUG)
            Serial.print("OneWire Device ");
            Serial.print((int)device_index);
            Serial.print(": At address ");
            for(uint8_t addr_index=0;addr_index<8;++addr_index)
            {
                if(addr.addr[addr_index]<0x10) Serial.print('0');
                Serial.print((int)addr.addr[addr_index],HEX);
            }
            Serial.println();
#endif
        }
        else
        {
            need_new_address=false;
#if defined(DS1820_DEBUG)
            Serial.print("OneWire Device ");
            Serial.print((int)device_index);
            Serial.println(": Can't get address");
#endif
        }
    }
}

void sensor_ds1820::update()
{
    unsigned long curr_millis=millis();
    if(curr_millis-prev_millis<update_every_ms)
        return;

    prev_millis=curr_millis;

    dtlib.requestTemperatures();
    for(size_t index=0;index<onewire_addresses.size();++index)
    {
        auto &addr=onewire_addresses.at(index);
        float temperature=dtlib.getTempC(addr.addr);
        float& last_temperature=last_temperatures.at(index);
        if((temperature!=last_temperature || always_notify) && temperature!=-127.0)
        {
            last_temperature=temperature;
            if(on_temperature_changed)
                on_temperature_changed(index,temperature,vnode_offset);
        }
#if defined(DS1820_DEBUG)
        Serial.print("Device ");
        Serial.print(index);
        Serial.print(" has temperature ");
        Serial.println(temperature);
#endif
    }
}
