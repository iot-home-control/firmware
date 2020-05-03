project('MCP320x', 'cpp')

sdk=subproject('esp8266_sdk')
core=sdk.get_variable('core')

arduino=subproject('esp8266_arduino')
spi=arduino.get_variable('SPI')

MCP320x_includes = include_directories(['src'])
MCP320x_src = ['src/Mcp320x.cpp']
MCP320x_lib = static_library('MCP320x', MCP320x_src, dependencies: [core, spi], include_directories: MCP320x_includes)

lib = declare_dependency(include_directories: MCP320x_includes, link_with: MCP320x_lib)
