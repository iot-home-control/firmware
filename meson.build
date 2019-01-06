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
    ArduinoJson.get_variable('lib')
]

cpp_base = [
    'components/node_base.cpp',
    'components/mqtt_handler.cpp',
    'components/updater_ota.cpp',
    'components/wifi_connector.cpp',
    'components/trampoline_magic.cpp',
    'components/periodic_message_poster.cpp',
    'components/config_loader.cpp'
]

deps_led_strip = deps_base + [
    ArduinoJson.get_variable('lib'),
    DHTSensorLibrary.get_variable('lib'),
    NeoPixelBus.get_variable('lib'),
    RotaryEncoder.get_variable('lib')
]

led_strip_exe = executable('led_strip.elf', cpp_base + ['node_led_strip.cpp', 'io/led_strip.cpp', 'io/gpio_pin.cpp', 'io/rotary_encoder.cpp', 'io/button.cpp'], dependencies: deps_led_strip)

led_strip_bin = custom_target('led_stip.bin',
  input : led_strip_exe,
  output : 'led_strip.bin',
  command : sdk.get_variable('esptoolize'),
  build_by_default: true,
)

deps_temperature_sensor = deps_base + [
    DHTSensorLibrary.get_variable('lib'),
    OneWire.get_variable('lib'),
    ArduinoTemperatureControlLibrary.get_variable('lib'),
]

temperature_sensor_exe = executable('temperature_sensor.elf', cpp_base + ['node_temperature_sensor.cpp', 'io/sensor_dht22.cpp', 'io/sensor_ds1820.cpp'], dependencies: deps_temperature_sensor)

temperature_sensor_bin = custom_target('temperature_sensor.bin',
  input : temperature_sensor_exe,
  output : 'temperature_sensor.bin',
  command : sdk.get_variable('esptoolize'),
  build_by_default: true,
)


deps_power_switch = deps_base + [
]

power_switch_exe = executable('power_switch.elf', cpp_base + ['node_power_switch.cpp', 'io/button.cpp', 'io/gpio_pin.cpp', 'io/relay.cpp'], dependencies: deps_power_switch)

power_switch_bin = custom_target('power_switch.bin',
  input : power_switch_exe,
  output : 'power_switch.bin',
  command : sdk.get_variable('esptoolize'),
  build_by_default: true,
)

deps_switch = deps_base + [
]

switch_exe = executable('switch.elf', cpp_base + ['node_switch.cpp', 'io/feedback_button.cpp', 'io/gpio_pin.cpp'], dependencies: deps_switch)

switch_bin = custom_target('switch.bin',
  input : switch_exe,
  output : 'switch.bin',
  command : sdk.get_variable('esptoolize'),
  build_by_default: true,
)

deps_wifi_indicator = deps_base + [
    arduino.get_variable('SPI'),
    Adafruit_GFX_Library.get_variable('lib'),
    Adafruit_PCD8544_Nokia_5110_LCD_library.get_variable('lib')
]

wifi_indicator_exe = executable('wifi_indicator.elf', cpp_base + ['node_wifi_indicator.cpp'], dependencies: deps_wifi_indicator)

wifi_indicator_bin = custom_target('wifi_indicator.bin',
  input : wifi_indicator_exe,
  output : 'wifi_indicator.bin',
  command : sdk.get_variable('esptoolize'),
  build_by_default: true,
)
