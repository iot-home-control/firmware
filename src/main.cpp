// kate: hl C++;
// vim: filetype=c++:

#include "components/node_base.h"

#if defined(SUPPORT_SENSORS)
#include "io/sensor_ds1820.h"
#include "io/sensor_dht22.h"
#include "io/sensor_bmp180.h"
#include "io/sensor_mcp320x.h"
#endif
#include "io/relay.h"
#include "io/gpio_pin.h"
#include "components/config_loader.h"
#include <ArduinoJson.h>

class node: public node_base
{
public:
    void setup() override;
    void loop() override;
    void on_wifi_connected() override;
private:
    config_loader loader;
};

void node::setup()
{
    node_base::setup();

    pinMode(0, INPUT_PULLUP);

#if defined(SUPPORT_SENSORS)
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

    loader.install_factory("mcp3208", 0, [this](const JsonObject &obj, int vnode_id) -> ticker_component*
    {
        int cs_pin = -1;
        int refresh_rate = 15*60;
        bool always_notify = false;

        if(obj.containsKey("cs_pin") && obj["cs_pin"].is<int>())
        {
            cs_pin = obj["cs_pin"].as<int>();
        }
        else
        {
            Serial.println("Can't create MCP3208. No CS Pin set.");
            return nullptr;
        }
        if(obj.containsKey("rate") && obj["rate"].is<int>())
            refresh_rate = obj["rate"].as<int>();
        if(obj.containsKey("always_notify") && obj["always_notify"].is<bool>())
            always_notify = obj["always_notify"].as<bool>();

        gpio_pin *cs_gpio = new gpio_pin();
        cs_gpio->begin(cs_pin, gpio_pin::pin_out);

        const uint16_t adc_vref = 3300; // 3.3V Vref;
        sensor_mcp320x<8> *mcp = new sensor_mcp320x<8>();
        mcp->on_value_changed = [this] (char channel, uint16_t raw, uint16_t analog) {
            float soil_moisture = (1 - (float)analog / 4096) * 100;
            mqtt.publish(get_state_topic("soilmoisture", channel),"local,"+String(soil_moisture));
            Serial.printf("Channel %d: raw %d analog %d\n", channel, raw, analog);
        };
        mcp->begin(cs_pin, adc_vref, 3, refresh_rate*1000, always_notify);

        return mcp;
    });
#endif

    loader.install_factory("switch", 0, [this](const JsonObject &obj, int vnode_id) -> ticker_component*
    {
        bool invert_relay = false;
        bool invert_led = false;
        bool start_on = false;
        char relay_pin = -1;
        char led_pin = -1;
        char button_pin = -1;
        bool button_pullup = true;
        char toggle_pin = -1;
        bool toggle_pullup = true;

        if(!obj.containsKey("relay_pin") || !obj["relay_pin"].is<int>())
        {
            Serial.println("Can't create relay. relay_pin is not set or not an int");
            return nullptr;
        }
        else
        {
            relay_pin = obj["relay_pin"].as<int>();
        }

        relay *r = new relay("switch");

        if(obj.containsKey("invert_relay") && obj["invert_relay"].is<bool>())
            invert_relay = obj["invert_relay"].as<bool>();
        if(obj.containsKey("led_active_low") && obj["led_active_low"].is<bool>())
            invert_led = obj["invert_led"].as<bool>();
        if(obj.containsKey("start_on") && obj["start_on"].is<bool>())
            start_on = obj["start_on"].as<bool>();
        if(obj.containsKey("led_pin") && obj["led_pin"].is<int>())
            led_pin = obj["led_pin"].as<int>();
        if(obj.containsKey("button_pin") && obj["button_pin"].is<int>())
            button_pin = obj["button_pin"].as<int>();
        if(obj.containsKey("button_pullup") && obj["button_pullup"].is<bool>())
            button_pullup = obj["button_pullup"].as<bool>();
        if(obj.containsKey("toggle_pin") && obj["toggle_pin"].is<int>())
            toggle_pin = obj["toggle_pin"].as<int>();
        if(obj.containsKey("toggle_pullup") && obj["toggle_pullup"].is<bool>())
            toggle_pullup = obj["toggle_pullup"].as<bool>();


        r->begin(this, vnode_id, relay_pin, led_pin, button_pin, button_pullup, toggle_pin, toggle_pullup, start_on, invert_relay, invert_led);

        return r;
    });

    loader.begin(components, digitalRead(0)==LOW);

    webserver.on("/config", [this]
    {
        webserver.send(200, "application/json", loader.dump());
    });
}

void node::loop()
{
    node_base::loop();
    node_base::wait_for_loop_timing();
}

void node::on_wifi_connected()
{
    node_base::on_wifi_connected();

    if(loader.check(device_id, CONFIG_SERVER))
    {
        Serial.println("/config.json was updated. Reboot...");
        ESP.restart();
    }
}

class node node;

void setup()
{
    node.setup();
}

void loop()
{
    node.loop();
}
