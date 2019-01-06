project('RotaryEncoder', 'cpp')

sdk=subproject('esp8266_sdk')
core=sdk.get_variable('core')

RotaryEncoder_includes = include_directories(['.'])
RotaryEncoder_src = ['RotaryEncoder.cpp']
RotaryEncoder_lib = static_library('RotaryEncoder', RotaryEncoder_src, dependencies: core, include_directories: RotaryEncoder_includes)

lib = declare_dependency(include_directories: RotaryEncoder_includes, link_with: RotaryEncoder_lib)
