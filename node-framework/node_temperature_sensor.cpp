// kate: hl C++;
// vim: filetype=c++:

#include "node_base.h"

#include "sensor_ds1820.h"
#include "sensor_dht22.h"
#include "config_loader.h"
#include <ArduinoJson.h>

class node_temperature_sensor: public node_base
{
public:
    void setup() override;
    void loop() override;
    void on_wifi_connected() override;
private:
    config_loader loader;
};

void node_temperature_sensor::setup()
{
    node_base::setup();

    pinMode(0, INPUT_PULLUP);

    loader.install_factory("dht22", 0, [this](const JsonObject &obj, int vnode_id) -> ticker_component*
    {
        if(!obj.containsKey("pin") || !obj["pin"].is<int>())
        {
            Serial.println("Can't create dht22. Pin is not set or not an int");
            return nullptr;
        }
        int refresh_rate = 10;
        if(obj.containsKey("rate") && obj["rate"].is<int>())
            refresh_rate = obj["rate"].as<int>();
        int pin = obj["pin"].as<int>();
        sensor_dht22 *sensor = new sensor_dht22();
        sensor->begin(pin, refresh_rate*1000);

        sensor->on_temperature_changed=[this,vnode_id](const float v)
        {
            Serial.printf("DHT22 #%d Temperature changed: ", vnode_id);
            Serial.println(v);
            mqtt.publish(get_state_topic("temperature",vnode_id),"local,"+String(v));
        };

        sensor->on_humidity_changed=[this,vnode_id](const float v)
        {
            Serial.printf("DHT22 #%d Humidity changed: ", vnode_id);
            Serial.println(v);
            mqtt.publish(get_state_topic("humidity",vnode_id),"local,"+String(v));
        };

        return sensor;
    });

    loader.install_factory("ds1820", 0, [this](const JsonObject &obj, int vnode_id) -> ticker_component*
    {
        if(!obj.containsKey("pin") || !obj["pin"].is<int>())
        {
            Serial.println("Can't create ds1820. Pin is not set or not an int");
            return nullptr;
        }
        int refresh_rate = 10;
        if(obj.containsKey("rate") && obj["rate"].is<int>())
            refresh_rate = obj["rate"].as<int>();
        int pin = obj["pin"].as<int>();

        sensor_ds1820 *sensor = new sensor_ds1820();
        sensor->begin(pin, refresh_rate*1000, vnode_id);
        sensor->on_temperature_changed=[this](const uint8_t index, const float v, const int vnode_offest)
        {
            Serial.print("Temperature on DS1820 #");
            Serial.print(index);
            Serial.print(" ");
            Serial.println(v);
            mqtt.publish(get_state_topic("temperature",vnode_offest+index),"local,"+String(v));
        };

        return sensor;
    });

    loader.begin(components, digitalRead(0)==LOW);
}

void node_temperature_sensor::loop()
{
    node_base::loop();
    node_base::wait_for_loop_timing();
}

void node_temperature_sensor::on_wifi_connected()
{
    node_base::on_wifi_connected();

    if(loader.check(device_id, CONFIG_SERVER))
    {
        Serial.println("/config.json was updated. Reboot...");
        ESP.restart();
    }
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
