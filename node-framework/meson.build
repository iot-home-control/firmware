project('led-strip', 'cpp', default_options: ['buildtype=minsize'])
assert(meson.is_cross_build(), 'ESP8266 projects can only be built in a cross build environment.')

add_global_arguments('-Wno-non-virtual-dtor', language: 'cpp')

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

deps_led_strip = [
    sdk.get_variable('core'),
    arduino.get_variable('ESP8266WiFi'),
    arduino.get_variable('ArduinoOTA'),
    arduino.get_variable('ESP8266HTTPClient'),
    arduino.get_variable('ESP8266mDNS'),
    arduino.get_variable('ESP8266httpUpdate'),
    ArduinoJson.get_variable('lib'),
    PubSubClient.get_variable('lib'),
    DHTSensorLibrary.get_variable('lib'),
    NeoPixelBus.get_variable('lib'),
    RotaryEncoder.get_variable('lib')
]

led_strip_exe = executable('led_strip.elf', ['node_base.cpp', 'led_strip.cpp', 'gpio_pin.cpp', 'mqtt_handler.cpp', 'sensor_dht22.cpp', 'node_led_strip.cpp', 'updater_http.cpp', 'updater_ota.cpp', 'wifi_connector.cpp', 'rotary_encoder.cpp', 'button.cpp', 'trampoline_magic.cpp', 'periodic_message_poster.cpp'], dependencies: deps_led_strip)

led_strip_bin = custom_target('led_stip.bin',
  input : led_strip_exe,
  output : 'led_strip.bin',
  command : sdk.get_variable('esptoolize'),
  build_by_default: true,
)

deps_temperature_sensor = [
    sdk.get_variable('core'),
    arduino.get_variable('ESP8266WiFi'),
    arduino.get_variable('ArduinoOTA'),
    arduino.get_variable('ESP8266HTTPClient'),
    arduino.get_variable('ESP8266mDNS'),
    PubSubClient.get_variable('lib'),
    DHTSensorLibrary.get_variable('lib'),
    OneWire.get_variable('lib'),
    ArduinoTemperatureControlLibrary.get_variable('lib')
]

temperature_sensor_exe = executable('temperature_sensor.elf', ['node_base.cpp', 'node_temperature_sensor.cpp', 'sensor_dht22.cpp', 'mqtt_handler.cpp', 'updater_ota.cpp', 'wifi_connector.cpp', 'periodic_message_poster.cpp', 'sensor_ds1820.cpp'], dependencies: deps_temperature_sensor)

temperature_sensor_bin = custom_target('temperature_sensor.bin',
  input : temperature_sensor_exe,
  output : 'temperature_sensor.bin',
  command : sdk.get_variable('esptoolize'),
  build_by_default: true,
)


deps_power_switch = [
    sdk.get_variable('core'),
    arduino.get_variable('ESP8266WiFi'),
    arduino.get_variable('ArduinoOTA'),
    arduino.get_variable('ESP8266HTTPClient'),
    arduino.get_variable('ESP8266mDNS'),
    PubSubClient.get_variable('lib')
]

power_switch_exe = executable('power_switch.elf', ['node_base.cpp', 'node_power_switch.cpp', 'mqtt_handler.cpp', 'updater_ota.cpp', 'wifi_connector.cpp', 'button.cpp', 'gpio_pin.cpp', 'trampoline_magic.cpp', 'periodic_message_poster.cpp'], dependencies: deps_power_switch)

power_switch_bin = custom_target('power_switch.bin',
  input : power_switch_exe,
  output : 'power_switch.bin',
  command : sdk.get_variable('esptoolize'),
  build_by_default: true,
)

deps_wifi_indicator = [
    sdk.get_variable('core'),
    arduino.get_variable('ESP8266WiFi'),
    arduino.get_variable('ArduinoOTA'),
    arduino.get_variable('ESP8266mDNS'),
    arduino.get_variable('SPI'),
    PubSubClient.get_variable('lib'),
    Adafruit_GFX_Library.get_variable('lib'),
    Adafruit_PCD8544_Nokia_5110_LCD_library.get_variable('lib')
]

wifi_indicator_exe = executable('wifi_indicator.elf', ['node_base.cpp', 'node_wifi_indicator.cpp', 'mqtt_handler.cpp', 'updater_ota.cpp', 'wifi_connector.cpp', 'periodic_message_poster.cpp'], dependencies: deps_wifi_indicator)

switch_bin = custom_target('wifi_indicator.bin',
  input : wifi_indicator_exe,
  output : 'wifi_indicator.bin',
  command : sdk.get_variable('esptoolize'),
  build_by_default: true,
)
