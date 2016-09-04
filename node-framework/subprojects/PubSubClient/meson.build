project('PubSubClient', 'cpp')

sdk=subproject('esp8266_sdk')
core=sdk.get_variable('core')

PubSubClient_includes = include_directories(['src'])
PubSubClient_src = ['src/PubSubClient.cpp']
PubSubClient_lib = static_library('PubSubClient', PubSubClient_src, dependencies: core, include_directories: PubSubClient_includes)

lib = declare_dependency(include_directories: PubSubClient_includes, link_with: PubSubClient_lib)
