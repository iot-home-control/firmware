// kate: hl C++;
// vim: filetype=c++:

#include "components/node_base.h"

#include "io/relay.h"
#include "components/config_loader.h"
#include <ArduinoJson.h>

class node_power_switch: public node_base
{
public:
    node_power_switch();

    void setup() override;
    void loop() override;
    void on_wifi_connected() override;
private:
    config_loader loader;
};

node_power_switch::node_power_switch(): node_base()
{
}

void node_power_switch::setup()
{
    node_base::setup();

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
        if(obj.containsKey("invert_led") && obj["invert_led"].is<bool>())
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
}

void node_power_switch::loop()
{
    node_base::loop();
    node_base::wait_for_loop_timing();
}

void node_power_switch::on_wifi_connected()
{
    node_base::on_wifi_connected();

    if(loader.check(device_id, CONFIG_SERVER))
    {
        Serial.println("/config.json was updated. Reboot...");
        ESP.restart();
    }
}

node_power_switch node;

void setup()
{
    node.setup();
}

void loop()
{
    node.loop();
}
