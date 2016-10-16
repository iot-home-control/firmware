project('led-strip', 'cpp', default_options: ['buildtype=minsize'])
assert(meson.is_cross_build(), 'ESP8266 projects can only be built in a cross build environment.')

sdk=subproject('esp8266_sdk')
arduino=subproject('esp8266_arduino')
ArduinoJson=subproject('ArduinoJson')
PubSubClient=subproject('PubSubClient')
DHTSensorLibrary=subproject('DHTSensorLibrary')
NeoPixelBus=subproject('NeoPixelBus')
RotaryEncoder=subproject('RotaryEncoder')
OneWire=subproject('OneWire')
ArduinoTemperatureControlLibrary=subproject('ArduinoTemperatureControlLibrary')

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

led_strip_exe = executable('led_strip.elf', ['led_strip.cpp', 'gpio_pin.cpp', 'mqtt_handler.cpp', 'sensor_dht22.cpp', 'node_led_strip.cpp', 'updater_http.cpp', 'updater_ota.cpp', 'wifi_connector.cpp', 'rotary_encoder.cpp', 'button.cpp', 'trampoline_magic.cpp'], dependencies: deps_led_strip)

led_strip_bin = custom_target('led_stip.bin',
  input : led_strip_exe,
  output : 'led_strip.bin',
  command : sdk.get_variable('esptoolize')
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

temperature_sensor_exe = executable('temperature_sensor.elf', ['node_temperature_sensor.cpp', 'sensor_dht22.cpp', 'mqtt_handler.cpp', 'updater_ota.cpp', 'wifi_connector.cpp'], dependencies: deps_temperature_sensor)

temperature_sensor_bin = custom_target('temperature_sensor.bin',
  input : temperature_sensor_exe,
  output : 'temperature_sensor.bin',
  command : sdk.get_variable('esptoolize')
)
