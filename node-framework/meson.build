project('led-strip', 'cpp', default_options: ['buildtype=minsize'])
assert(meson.is_cross_build(), 'ESP8266 projects can only be built in a cross build environment.')

sdk=subproject('esp8266_sdk')
arduino=subproject('esp8266_arduino')
ArduinoJson=subproject('ArduinoJson')
PubSubClient=subproject('PubSubClient')
DHTSensorLibrary=subproject('DHTSensorLibrary')
NeoPixelBus=subproject('NeoPixelBus')
RotaryEncoder=subproject('RotaryEncoder')

deps = [
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

exe = executable('app.elf', ['led_strip.cpp', 'gpio_pin.cpp', 'mqtt_handler.cpp', 'sensor_dht22.cpp', 'node_led_strip.cpp', 'updater_http.cpp', 'updater_ota.cpp', 'wifi_connector.cpp', 'rotary_encoder.cpp', 'trampoline_magic.cpp'], dependencies: deps)

bin = custom_target('app.bin',
  input : exe,
  output : 'app.bin',
  command : sdk.get_variable('esptoolize')
)

