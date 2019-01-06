// kate: hl C++;
// vim: filetype=c++:

#include "node_base.h"

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Fonts/Picopixel.h>

/*
 * Display      ESP NodeMCU Devboard (GPIO)
 * RST   <----> D2 (4)
 * CE    <----> D8 (15)
 * DC    <----> D6 (12)
 * DIN   <----> D7 (13)
 * CLK   <----> D5 (14)
 * VCC   <----> 3.3V / 5V
 * LIGHT <----> GND/PWM Pin 0V -> On, VCC -> Off
 * GND   <----> GND
 */

Adafruit_PCD8544 display = Adafruit_PCD8544(12/*D6*/, 15/*D8*/, 4/*D2*/);

class rssi_indicator: public ticker_component
{
public:
    void begin(int row=0)
    {
        this->row=row;
    }

    void update()
    {
        if(!go)
            return;

        unsigned long now=millis();
        if(now-last_millis<1000)
            return;
        last_millis=now;

        display.fillRect(0, row*8, 84, 8, 0);
        display.setCursor(0, row*8);
        display.print("RSSI: ");
        display.print(WiFi.RSSI());
        display.println(" dBm");
        display.display();
    }

    void start()
    {
        go=true;
    }

    void stop()
    {
        go=false;
    }

private:
    int row=0;
    unsigned long last_millis=0;
    bool go=false;
};

class display_alive_indicator: public ticker_component
{
public:
    void begin(int row=0)
    {
        this->row=row;
    }

    void update()
    {
        unsigned long now=millis();
        if(now-last_millis<500)
            return;
        last_millis=now;

        display.fillRect(0, row*8, 48, 8, 0);
        display.setCursor(0, row*8);
        display.print(indicator[index]);
        display.display();
        index++;
        if(index>3) index=0;
    }

private:
    int row=0;
    unsigned long last_millis=0;
    int index=0;
    const char indicator[5]="/-\\|";
};

class node_wifi_indicator: public node_base
{
public:
    void setup() override;
    void loop() override;
    void on_wifi_connected() override;
    void on_wifi_disconnected() override;
private:
    rssi_indicator rssi;
    display_alive_indicator alive;
};



void node_wifi_indicator::setup()
{
    node_base::setup();

    rssi.begin(3);
    components.push_back(&rssi);
    alive.begin(5);
    components.push_back(&alive);

    display.begin();
    display.clearDisplay();
    display.setContrast(50);

    display.println("This is:");
    display.println(device_id);
    display.display();
}

void node_wifi_indicator::loop()
{
    node_base::loop();
    node_base::wait_for_loop_timing();
}

void node_wifi_indicator::on_wifi_connected()
{
    node_base::on_wifi_connected();
    display.clearDisplay();
    display.println("WiFi Connected");
    display.println("IP:");
    display.println(WiFi.localIP());
    display.display();
    rssi.start();
}

void node_wifi_indicator::on_wifi_disconnected()
{
    node_base::on_wifi_disconnected();
    display.clearDisplay();
    display.println("WiFi Disconnected");
    display.display();
    rssi.stop();
}

node_wifi_indicator node;

void setup()
{
    node.setup();
}

void loop()
{
    node.loop();
}
