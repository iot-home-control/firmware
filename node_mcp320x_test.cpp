#include "components/node_base.h"

#include <memory>
#include <functional>
#include <Mcp320x.h>

#include "io/gpio_pin.h"
#include "io/feedback_button.h"

class node_mcp320x_test: public node_base
{
public:
    node_mcp320x_test();

    void setup();
    void loop();
private:
    
    #define SPI_CS 16
    gpio spi_cs; // SPI slave select 16
    auto adc_vref = 3300; // 3.3V Vref
    auto ac_clk = 100000; // SPI clock 1.6MHz*/
    
    MCP3208 adc(adc_vref, SPI_CS);

};

node_mcp320x_test::node_mcp320x_test(): node_base()
{

}

void node_mcp320x_test::setup()
{
    node_base::setup();
    spi_cs.begin(SPI_CS,  gpio_pin::pin_out);
    components.push_back(&spi_cs);
    SPISettings settings(adc_clk, MSBFIRST, SPI_MODE0);
    SPI.begin();
    SPI.beginTransaction(settings);
}

void node_mcp320x_test::loop()
{
    node_base::loop();
    uint32_t t1;
    uint32_t t2;

    // start sampling
    Serial.println("Reading...");
    spi_cs.write(HIGH);

    t1 = micros();
    uint16_t raw = adc.read(MCP3208::Channel::SINGLE_0);
    t2 = micros();

    // get analog value
    uint16_t val = adc.toAnalog(raw);

    // readed value
    Serial.print("value: ");
    Serial.print(raw);
    Serial.print(" (");
    Serial.print(val);
    Serial.println(" mV)");

    // sampling time
    Serial.print("Sampling time: ");
    Serial.print(static_cast<double>(t2 - t1) / 1000, 4);
    Serial.println("ms");
    
    spi_cs.write(LOW);
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
