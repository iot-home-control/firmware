// kate: hl C++;
// vim: filetype=c++: 

#include "node_base.h"

#include <DallasTemperature.h>
#include "sensor_dht22.h"

sensor_dht22 dht22;

class node_temperature_sensor: public node_base
{
public:
    void setup() override;
    void loop() override;
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
