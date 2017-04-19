#include "node_base.h"

node_base::node_base(bool serial_alternate_rx_tx): serial_alternate_pins(serial_alternate_rx_tx)
{
    device_id="esp8266-"+String(ESP.getChipId(), HEX);
}

node_base::~node_base()
{
}

void node_base::setup() {
    // Init Hardware Serial on alternate Pins (RX:15/TX:13) instead of the
    // default pins, as the default RX pin is also I2S aka DMA ouput, which is
    // used by the LEDs
    Serial.begin(115200);
    if(serial_alternate_pins)
        Serial.pins(15,13);

    // Wait for the Serial port to become ready
    while(!Serial)
    {
        delay(10);
    }
    Serial.println();

    Serial.print("Built on ");
    Serial.print(__DATE__);
    Serial.print(", ");
    Serial.println(__TIME__);

    Serial.print("Device-Id: ");
    Serial.println(device_id);

    wifi.begin(WIFI_SSID, WIFI_PASS);
    wifi.on_connected=[&]
    {
        Serial.print("Connected to WiFi (");
        Serial.print(WIFI_SSID);
        Serial.println(")");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        ota.begin();
    };

    ota.set_password(OTA_PASS);

    mqtt.begin(MQTT_HOST,device_id,MQTT_USER,MQTT_PASS);
    mqtt.on_connected=std::bind(&node_base::on_mqtt_connected, this);
    mqtt.on_disconnected=std::bind(&node_base::on_mqtt_disconnected, this);

    message_poster.begin(&mqtt, "/alive", device_id, 60000);
    components.push_back(&message_poster);
}

void node_base::loop() {
    loop_start_ms=millis();
    wifi.update();
    ota.update();
    mqtt.update();
    for(auto& component: components)
    {
        component->update();
    }
}

void node_base::wait_for_loop_timing()
{
    unsigned long used_ms=millis()-loop_start_ms;
    if(used_ms<cycle_time_ms)
        delay(cycle_time_ms-used_ms);
}

String node_base::get_state_topic(const String &type)
{
    return "/"+type+"/"+device_id+"/state";
}

String node_base::get_action_topic(const String &type, const String &alt_suffix)
{
    String base = "/"+type+"/"+device_id;
    if(alt_suffix=="")
    {
        return base+"/action";
    }
    else
    {
        return base+"/"+alt_suffix;
    }

}

void node_base::on_wifi_connected()
{
    Serial.println("Connected to MQTT");
}

void node_base::on_wifi_disconnected()
{
    Serial.println("Disconnected from MQTT");
}

void node_base::on_mqtt_connected()
{

}

void node_base::on_mqtt_disconnected()
{

}

