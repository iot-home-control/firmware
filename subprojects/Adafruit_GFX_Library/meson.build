project('Adafruit_GFX_Library', 'cpp')

sdk=subproject('esp8266_sdk')
core=sdk.get_variable('core')

Adafruit_GFX_Library_includes = include_directories(['.', './Fonts'])
Adafruit_GFX_Library_src = ['Adafruit_GFX.cpp']
Adafruit_GFX_Library_lib = static_library('Adafruit_GFX_Library', Adafruit_GFX_Library_src, dependencies: [core] , include_directories: Adafruit_GFX_Library_includes)

lib = declare_dependency(include_directories: Adafruit_GFX_Library_includes, link_with: Adafruit_GFX_Library_lib)
