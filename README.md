# Home Control

Home Control is a no-cloud Internet of Things solution. 

Home Control has 3 Components
- the [System Backend](https://github.com/iot-home-control/system-backend)
- the [Web Frontend](https://github.com/iot-home-control/frontend)
- the Firmware (this repository)

The Home Control Firmware is a multi-purpose firmware for ESP8266-based IOT devices.
The firmware supports sensors (DHT22, DS18B20, BMP180, Capacitive Soil Moisture Sensor via MCP3208 ADC) and all kinds of binary GPIO actors such as relays boards.
It runs on custom boards based on NodeMCUv2 (ESP12-E, ESP12-F) as well as on commercial IOT devices namely Sonoff Basic and Sonoff S20 (probably, other products will work, too, but have not been tested so far).
Although, it would be possible to connect sensors to Sonoff devices, but for safety reasons it is not recommended and disabled.

## Installation Guide
### Requirements
To compile the firmware you need:
- python3 (as requirement for platformio)
- [PlatformIO](https://platformio.org) (we recommend installing it into a Python virtualenv)

### Setup
1. Unpack the downloaded release file (or clone this repository) somewhere on your computer.
1. Create a config file in the `src/` directory.
   We provide a `config.h.example` to help you getting started.
   See [the build configuration section](#Build-Configuration) for more information.
1. Build the firmware by running `pio run` in a terminal in the project root.
   If you want to save some time and/or disk space and only build the firmware for a specific supported board (see `platformio.ini` for supported boards, sections starting with `env:`) you can do so by running `pio run --board=<name>` instead.
1. [Upload](#upload) the firmware to your device(s).   
### Build Configuration
The Firmware is configured via a C header file. The following options are supported:
- `WIFI_SSID`, `WIFI_PASS`: SSID and password of your WiFi network.
  In case your WiFi network doesn't use a password just leave the password entry blank (an empty string).
- `OTA_PASS`: The password used for over-the-air firmware updates.
- `MQTT_HOST`, `MQTT_USER`, `MQTT_PASS`: MQTT connection details.
  The `MQTT_HOST` can be either an IP address, or a hostname which can be resolved via DNS.
  In case your MQTT broker doesn't use authentication you can leave `MQTT_USER` and/or `MQTT_PASS` blank (an empty string).
- `CONFIG_SERVER`: The host where your system-backend instance is running.
  It can be either an IP address, or a hostname which can be resolved via DNS.
  This is used to check for new configuration files.

### Upload
Platformio has the ability to upload either via serial console or OTA.
However, we provide a bash script `upload.sh`, which is more convenient, than using platformio.

Usage:  
`./upload.sh <BOARD> serial [PORT] [BAUD]`  
`./upload.sh <BOARD> ota <NAME> [PASS] [-p PORT]`

Board can be 
- nodemcuv2
- sonoff_basic
- sonoff_s20

Name can be
- an IP address 
- a DNS name, which is `esp8266-<DEVICE-ID>`
- an alias configured in `upload_aliases.json`

#### Aliases
To use the "Upload aliases" feature you must create a `upload_aliases.json` file.
See `upload_aliases.json.example` for an example file.

### Runtime Configuration

Sensors and actors on devices with Home Control Firmware are configured at runtime.
When a device boots up it checks whether there is a new configuration on the config server.
If so the device saves the configuration to the local filesystem and reboots to apply it.
To configure your device, put a file named `esp8266-<DEVICE-ID>.json` in a folder `config/` in the webroot of the config server (see [Build configuration](#Build Configuration)).

The configuration must contain the followings keys
- `name` a human-readable identifier
- `version` an integer, which is used to check whether the config has changed
- `components` a list of components (see examples below or [detailed documentation](####Components))

Any device can have multiple components of the same type or of distinct types.
For any component of the same type, a virtual node id (`vnode_id`) is incremented.
The ids will be allocated in the order of the components in the configuration starting at 0.
In the Web Frontend, this `vnode_id` is needed to configure a device for the Home Control IOT System.

#### Examples
All mentioned pins correspond to the Arduino pin naming scheme.  

Sonoff relay device
```json
{
    "name": "Sonoff Basic/S20",
    "version": 1,
    "components": [
        {
            "type": "switch",
            "relay_pin": 12,
            "led_pin": 13,
            "led_active_low": true,
            "button_pin": 0,
            "start_on": false,
            "button_pullup": false
        }
    ]
}
```
Dual DHT22 Temperature and Humidity sensor
```JSON
{
    "name": "TWO DHT22",
    "version": 0,
    "components": [
        {
            "type": "dht22",
            "pin": 5,
            "rate": 60,
            "always_notify": true
        },
        {
            "type": "dht22",
            "pin": 4,
            "rate": 60,
            "always_notify": true
        }
    ]
}
```

#### Components
Most component configuration parameters are optional.
Required ones are marked as such.

- `dht22`:
  A combined temperature/humidity sensor
    * `pin`: Number, *Required*, IO pin the sensor is connected to.
    * `rate`: Number, Delay between to sensor(s) polls (in seconds).
    * `always_notify`: Boolean, If enabled always send the current value, otherwise it will only be sent if it changed.
- `ds1820`:
  A OneWire temperature sensor. Multiple sensors can be daisy-chained on a single pin. They will automatically get assigned vnode ids, as mentioned above.
    * `pin`: Number, *Required*, IO pin the sensor(s) are connected to.
    * `rate`: Number, Delay between two sensor(s) polls (in seconds).
    * `always_notify`: Boolean, If enabled always send the current value, otherwise it will only be sent if it changed.
- `bmp180`:
  An air pressure and temperature sensor. Connected via SPI.
    * `rate`: Number, Delay between two sensor(s) polls (in seconds).
    * `always_notify`: Boolean, If enabled always send the current value, otherwise it will only be sent if it changed.
    * `altitude`: Number, Device altitude above sea level in meters.
      Used to calculate sea-level equivalent air pressure.
      Without this the device will only report the temperature.
- `mcp3208`:
  An MCP3208 ADC connected via SPI. Used for Capacitive Soil Moisture Sensors.
    * `cs_pin`: Number, *Required*, IO pin the sensor chip select pin is connected to.
    * `rate`: Number, Delay between two sensor(s) polls (in seconds).
    * `always_notify`: Boolean, If enabled always send the current value, otherwise it will only be sent if it changed.
- `switch`:
  A relay with optional local control switch and/or button and status LED.
    * `relay_pin`: Number, *Required*, IO pin the relay contact is connected to.
    * `start_on`: Boolean, If true the relay will be turned on when the device starts.  
    * `invert_relay`: Boolean, If true the relay will be inverted which means the coil will be energized when the relay should turn off instead when it should turn on.
      This is useful if the relay is switched via a transistor or a pull up/down resistor.
    * `led_pin`: Number, IO pin a local status LED is connected to.
    * `led_active_low`: Boolean, If true the LED will be turned on when pin is low.
      Useful for LEDs with switching transistors or pull up resistors.
    * `button_pin`: Number, IO pin a momentary switch for local control is connected to.
    * `button_pullup`: Boolean, If true the on chip pull-up resistor is enabled for the pin.
    * `toggle_pin`: Number, IO pin a toggle switch for local control is connected to.
      It behaves like a normal multiway light switch.
    * `toggle_pullup`: Boolean, If true the on chip pull-up resistor is enabled for the pin.
