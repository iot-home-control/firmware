project('OneWire', 'cpp')

sdk=subproject('esp8266_sdk')
core=sdk.get_variable('core')

OneWire_includes = include_directories(['.'])
OneWire_src = ['OneWire.cpp']
OneWire_lib = static_library('OneWire', OneWire_src, dependencies: core, include_directories: OneWire_includes)

lib = declare_dependency(include_directories: OneWire_includes, link_with: OneWire_lib)
