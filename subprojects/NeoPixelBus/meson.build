project('NeoPixelBus', 'cpp')

sdk=subproject('esp8266_sdk')
core=sdk.get_variable('core')

arduino=subproject('esp8266_arduino')

NeoPixelBus_includes = include_directories(['src'])
# find  -name '*.cpp' | sort | sed "s/|\.\/\(.*\)/'\1',/"
NeoPixelBus_src = [
    'src/internal/HsbColor.cpp',
    'src/internal/HslColor.cpp',
    'src/internal/HtmlColor.cpp',
    'src/internal/HtmlColorNames.cpp',
    'src/internal/HtmlColorNameStrings.cpp',
    'src/internal/HtmlColorShortNames.cpp',
    'src/internal/NeoEsp8266UartMethod.cpp',
    'src/internal/NeoGamma.cpp',
    'src/internal/NeoPixelAnimator.cpp',
    'src/internal/RgbColor.cpp',
    'src/internal/RgbwColor.cpp',
]
NeoPixelBus_lib = static_library('NeoPixelBus',
                                 NeoPixelBus_src,
                                 dependencies : [core, arduino.get_variable('SPI')],
                                 include_directories : NeoPixelBus_includes)

lib = declare_dependency(include_directories: NeoPixelBus_includes,
                         link_with : NeoPixelBus_lib,
                         dependencies : [arduino.get_variable('SPI')])
