project('BMP180', 'cpp')

sdk=subproject('esp8266_sdk')
core=sdk.get_variable('core')

arduino=subproject('esp8266_arduino')
wire=arduino.get_variable('Wire')

BMP180_includes = include_directories(['src'])
BMP180_src = ['src/SFE_BMP180.cpp']
BMP180_lib = static_library('BMP180', BMP180_src, dependencies: [core, wire], include_directories: BMP180_includes)

lib = declare_dependency(include_directories: BMP180_includes, link_with: BMP180_lib)
