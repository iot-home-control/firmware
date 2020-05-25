// kate: hl C++;
// vim: filetype=c++:

#include "components/node_base.h"

#include "io/sensor_ds1820.h"
#include "io/sensor_dht22.h"
#include "io/sensor_bmp180.h"
#include "components/config_loader.h"
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
        bool always_notify = false;
        if(obj.containsKey("always_notify") && obj["always_notify"].is<bool>())
            always_notify = obj["always_notify"].as<bool>();
        int pin = obj["pin"].as<int>();
        sensor_dht22 *sensor = new sensor_dht22();
        sensor->begin(pin, refresh_rate*1000, always_notify);

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
        bool always_notify = false;
        if(obj.containsKey("always_notify") && obj["always_notify"].is<bool>())
            always_notify = obj["always_notify"].as<bool>();
        int pin = obj["pin"].as<int>();

        sensor_ds1820 *sensor = new sensor_ds1820();
        sensor->begin(pin, refresh_rate*1000, always_notify, vnode_id);
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

    loader.install_factory("bmp180", 0, [this](const JsonObject &obj, int vnode_id) -> ticker_component*
    {
        int refresh_rate = 10;
        if(obj.containsKey("rate") && obj["rate"].is<int>())
            refresh_rate = obj["rate"].as<int>();
        bool always_notify = false;
        if(obj.containsKey("always_notify") && obj["always_notify"].is<bool>())
            always_notify = obj["always_notify"].as<bool>();
        float altitude = NAN;
        if(obj.containsKey("altitude") && obj["altitude"].is<double>())
            altitude = obj["altitude"].as<double>();

        sensor_bmp180 *sensor = new sensor_bmp180();
        sensor->begin(refresh_rate*1000, always_notify, altitude);
        sensor->on_temperature_changed = [this, vnode_id](const float value)
        {
            Serial.printf("BMP180 #%d Temperature changed: ", vnode_id);
            Serial.print(value);
            Serial.println(" °C");
            mqtt.publish(get_state_topic("temperature", vnode_id),"local,"+String(value));
        };

        sensor->on_pressure_changed = [this, vnode_id](const float absolute, const float sea_level)
        {
            Serial.printf("BMP180 #%d Pressure changed: ", vnode_id);
            Serial.print(absolute);
            Serial.print("mbar (sea-level: ");
            Serial.print(sea_level);
            Serial.println("mbar)");
            mqtt.publish(get_state_topic("pressure", vnode_id),"local,"+String(sea_level));
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
