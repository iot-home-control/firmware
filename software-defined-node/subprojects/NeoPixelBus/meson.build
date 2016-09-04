project('NeoPixelBus', 'cpp')

sdk=subproject('esp8266_sdk')
core=sdk.get_variable('core')

NeoPixelBus_includes = include_directories(['src'])
NeoPixelBus_src = ['src/internal/HslColor.cpp', 'src/internal/RgbwColor.cpp', 'src/internal/HsbColor.cpp', 'src/internal/HtmlColorNameStrings.cpp', 'src/internal/HtmlColor.cpp', 'src/internal/RgbColor.cpp', 'src/internal/HtmlColorShortNames.cpp', 'src/internal/NeoGamma.cpp', 'src/internal/HtmlColorNames.cpp', 'src/internal/NeoPixelAnimator.cpp', 'src/internal/NeoEsp8266UartMethod.cpp']
NeoPixelBus_lib = static_library('NeoPixelBus', NeoPixelBus_src, dependencies: core, include_directories: NeoPixelBus_includes)

lib = declare_dependency(include_directories: NeoPixelBus_includes, link_with: NeoPixelBus_lib)
