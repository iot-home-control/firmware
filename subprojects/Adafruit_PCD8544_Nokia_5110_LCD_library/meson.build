project('Adafruit_PCD8544_Nokia_5110_LCD_library', 'cpp')

sdk=subproject('esp8266_sdk')
core=sdk.get_variable('core')

arduino=subproject('esp8266_arduino')
spi=arduino.get_variable('SPI')

GFX=subproject('Adafruit_GFX_Library')
GFX_lib=GFX.get_variable('lib')

Adafruit_PCD8544_Nokia_5110_LCD_library_includes = include_directories(['.'])
Adafruit_PCD8544_Nokia_5110_LCD_library_src = ['Adafruit_PCD8544.cpp']
Adafruit_PCD8544_Nokia_5110_LCD_library_lib = static_library('Adafruit_PCD8544_Nokia_5110_LCD_library', Adafruit_PCD8544_Nokia_5110_LCD_library_src, dependencies: [core,GFX_lib,spi] , include_directories: Adafruit_PCD8544_Nokia_5110_LCD_library_includes)

lib = declare_dependency(include_directories: Adafruit_PCD8544_Nokia_5110_LCD_library_includes, link_with: Adafruit_PCD8544_Nokia_5110_LCD_library_lib)
