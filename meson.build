project('node-framework', 'cpp', default_options: ['buildtype=minsize'])
subdir('sdk_setup')

add_global_arguments(['-Wno-non-virtual-dtor'], language: 'cpp')

sdk=subproject('esp8266_sdk')
arduino=subproject('esp8266_arduino')
ArduinoJson=subproject('ArduinoJson')
PubSubClient=subproject('PubSubClient')
DHTSensorLibrary=subproject('DHTSensorLibrary')
NeoPixelBus=subproject('NeoPixelBus')
RotaryEncoder=subproject('RotaryEncoder')
OneWire=subproject('OneWire')
ArduinoTemperatureControlLibrary=subproject('ArduinoTemperatureControlLibrary')
Adafruit_GFX_Library=subproject('Adafruit_GFX_Library')
Adafruit_PCD8544_Nokia_5110_LCD_library=subproject('Adafruit_PCD8544_Nokia_5110_LCD_library')

deps_base = [
    sdk.get_variable('core'),
    arduino.get_variable('ESP8266WiFi'),
    arduino.get_variable('ArduinoOTA'),
    arduino.get_variable('ESP8266mDNS'),
    PubSubClient.get_variable('lib'),
    arduino.get_variable('ESP8266HTTPClient'),
    ArduinoJson.get_variable('lib'),
]

srcs_base = [
    'components/node_base.cpp',
    'components/mqtt_handler.cpp',
    'components/updater_ota.cpp',
    'components/wifi_connector.cpp',
    'components/trampoline_magic.cpp',
    'components/periodic_message_poster.cpp',
    'components/config_loader.cpp',
]

deps_led_strip = [
    ArduinoJson.get_variable('lib'),
    DHTSensorLibrary.get_variable('lib'),
    NeoPixelBus.get_variable('lib'),
    RotaryEncoder.get_variable('lib'),
]

srcs_led_strip = [
    'node_led_strip.cpp',
    'io/led_strip.cpp',
    'io/gpio_pin.cpp',
    'io/rotary_encoder.cpp',
    'io/button.cpp',
]

deps_temperature_sensor = [
    DHTSensorLibrary.get_variable('lib'),
    OneWire.get_variable('lib'),
    ArduinoTemperatureControlLibrary.get_variable('lib'),
]

srcs_temperature_sensor = [
    'node_temperature_sensor.cpp',
    'io/sensor_dht22.cpp',
    'io/sensor_ds1820.cpp',
]

deps_power_switch = [
]

srcs_power_switch = [
    'node_power_switch.cpp',
    'io/button.cpp',
    'io/gpio_pin.cpp',
    'io/relay.cpp',
]

deps_switch = [
]

srcs_switch = [
    'node_switch.cpp',
    'io/feedback_button.cpp',
    'io/gpio_pin.cpp',
]

deps_wifi_indicator = [
    arduino.get_variable('SPI'),
    Adafruit_GFX_Library.get_variable('lib'),
    Adafruit_PCD8544_Nokia_5110_LCD_library.get_variable('lib'),
]

srcs_wifi_indicator = [
    'node_wifi_indicator.cpp',
]

nodes = {
    'led_strip' : {
        'deps' : deps_led_strip,
        'srcs' : srcs_led_strip,
    },
    'temperature_sensor' : {
        'deps' : deps_temperature_sensor,
        'srcs' : srcs_temperature_sensor,
    },
    'power_switch' : {
        'deps' : deps_power_switch,
        'srcs' : srcs_power_switch,
    },
    'switch' : {
        'deps' : deps_switch,
        'srcs' : srcs_switch,
    },
    'wifi_indicator' : {
        'deps' : deps_wifi_indicator,
        'srcs' : srcs_wifi_indicator,
    },
    'led_test': {
        'deps' : deps_led_strip,
        'srcs' : ['node_led_test.cpp'],
    },
}

boardtype = get_option('boardtype')

foreach node, data : nodes
    prefix = node + '_' + boardtype
    exe = executable(prefix + '.elf', srcs_base + data['srcs'], dependencies : deps_base + data['deps'])
    bin = custom_target(prefix + '.bin',
        input : exe,
        output : prefix + '.bin',
        command : sdk.get_variable('esptoolize'),
        build_by_default : true,
    )
endforeach
