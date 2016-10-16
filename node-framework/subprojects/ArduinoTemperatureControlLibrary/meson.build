project('ArduinoTemperatureControlLibrary', 'cpp')

sdk=subproject('esp8266_sdk')
core=sdk.get_variable('core')

OneWire=subproject('OneWire')
OneWire_lib=OneWire.get_variable('lib')

ArduinoTemperatureControlLibrary_includes = include_directories(['.'])
ArduinoTemperatureControlLibrary_src = ['DallasTemperature.cpp']
ArduinoTemperatureControlLibrary_lib = static_library('ArduinoTemperatureControlLibrary', ArduinoTemperatureControlLibrary_src, dependencies: [core,OneWire_lib] , include_directories: ArduinoTemperatureControlLibrary_includes)

lib = declare_dependency(include_directories: ArduinoTemperatureControlLibrary_includes, link_with: ArduinoTemperatureControlLibrary_lib)
