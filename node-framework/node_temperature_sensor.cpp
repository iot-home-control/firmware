// kate: hl C++;
// vim: filetype=c++: 

#include "node_base.h"

#include "sensor_ds1820.h"
#include "sensor_dht22.h"

class node_temperature_sensor: public node_base
{
public:
    void setup() override;
    void loop() override;
private:
    sensor_dht22 dht22;
    sensor_ds1820 ds1820;
};

void node_temperature_sensor::setup()
{
    node_base::setup();
    dht22.begin(5,10000);
    dht22.on_temperature_changed=[this](const float v)
    {
        Serial.print("Temperature changed ");
        Serial.println(v);
        mqtt.publish(get_state_topic("temperature"),"local,"+String(v));
    };
    dht22.on_humidity_changed=[this](const float v)
    {
        Serial.print("Humidity changed ");
        Serial.println(v);
        mqtt.publish(get_state_topic("humidity"),"local,"+String(v));
    };
    components.push_back(&dht22);

    ds1820.begin(4,10000,1);
    ds1820.on_temperature_changed=[this](const uint8_t index, const float v, const int vnode_offest)
    {
        Serial.print("Temperature on DS1820 #");
        Serial.print(index);
        Serial.print(" ");
        Serial.println(v);
        mqtt.publish(get_state_topic("temperature",vnode_offest+index),"local,"+String(v));
    };
    components.push_back(&ds1820);
}

void node_temperature_sensor::loop()
{
    node_base::loop();
    node_base::wait_for_loop_timing();
}

node_temperature_sensor node;

void setup()
{
    node.setup();
}

void loop()
{
    node.loop();
}
