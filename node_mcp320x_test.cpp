#include "components/node_base.h"

#include <memory>
#include <functional>

#include "io/sensor_mcp320x.h"
#include "io/gpio_pin.h"

class node_mcp320x_test: public node_base
{
public:
    node_mcp320x_test();

    void setup();

private:
    
    #define SPI_CS 16
    gpio_pin spi_cs; // SPI slave select 16
    uint16_t adc_vref = 3300; // 3.3V Vref
    int adc_clk = 100000; // SPI clock 1.6MHz*/
    sensor_mcp320x<8> mcp;

};

node_mcp320x_test::node_mcp320x_test(): node_base()
{

}

void node_mcp320x_test::setup()
{
    node_base::setup();
    spi_cs.begin(SPI_CS,  gpio_pin::pin_out);
    SPISettings settings(adc_clk, MSBFIRST, SPI_MODE0);
    SPI.begin();
    SPI.beginTransaction(settings);
    mcp.on_value_changed = [this] (char channel, uint16_t raw, uint16_t analog) {
        float soil_moisture = (1 -(float) analog / 4096) * 100;
        mqtt.publish(get_state_topic("soilmoisture", channel),"local,"+String(soil_moisture));
        Serial.printf("Channel %d: raw %d analog %d\n", channel, raw, analog);
    };
    mcp.begin(SPI_CS, adc_vref, 3, 30000, true);

    components.push_back(&mcp);
}



//mqtt.publish("/switch/"+client_id+"/state","local,reboot");

node_mcp320x_test node;

void setup()
{
    node.setup();
}

void loop()
{
    node.loop();
}
