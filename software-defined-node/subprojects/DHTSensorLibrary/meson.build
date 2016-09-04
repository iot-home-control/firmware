project('DHTSensorLibrary', 'cpp')

sdk=subproject('esp8266_sdk')
core=sdk.get_variable('core')

DHTSensorLibrary_includes = include_directories(['.'])
DHTSensorLibrary_src = ['DHT.cpp']
DHTSensorLibrary_lib = static_library('DHTSensorLibrary', DHTSensorLibrary_src, dependencies: core, include_directories: DHTSensorLibrary_includes)

lib = declare_dependency(include_directories: DHTSensorLibrary_includes, link_with: DHTSensorLibrary_lib)
