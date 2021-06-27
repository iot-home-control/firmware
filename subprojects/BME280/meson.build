project('BME280', 'cpp')

sdk=subproject('esp8266_sdk')
core=sdk.get_variable('core')

arduino=subproject('esp8266_arduino')
wire=arduino.get_variable('Wire')

BME280_includes = include_directories(['.'])
BME280_src = ['Adafruit_BME280.cpp']
BME280_lib = static_library('BME280', BME280_src, dependencies: [core, wire], include_directories: BME280_includes)

lib = declare_dependency(include_directories: BME280_includes, link_with: BME280_lib)
