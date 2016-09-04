project('esp8266_arduino', 'cpp')

esp8266_base = meson.get_cross_property('esp8266_base')

sdk=subproject('esp8266_sdk')
core=sdk.get_variable('core')

library_base = esp8266_base + '/libraries/'


ESP8266WiFi_src = []
foreach s: ['ESP8266WiFiAP.cpp', 'ESP8266WiFi.cpp', 'ESP8266WiFiGeneric.cpp', 'ESP8266WiFiMulti.cpp', 'ESP8266WiFiScan.cpp', 'ESP8266WiFiSTA.cpp', 'WiFiClient.cpp', 'WiFiClientSecure.cpp', 'WiFiServer.cpp', 'WiFiUdp.cpp']
  ESP8266WiFi_src += library_base+'/ESP8266WiFi/src/'+s
endforeach
ESP8266WiFi_includes = include_directories([library_base+'/ESP8266WiFi/src/'])
ESP8266WiFi_lib = static_library('ESP8266WiFi', ESP8266WiFi_src, dependencies: core, include_directories: ESP8266WiFi_includes)
ESP8266WiFi = declare_dependency(include_directories: ESP8266WiFi_includes, link_with: ESP8266WiFi_lib)


ESP8266mDNS_src = []
foreach s: ['ESP8266mDNS.cpp']
  ESP8266mDNS_src += library_base+'/ESP8266mDNS/'+s
endforeach
ESP8266mDNS_includes = include_directories([library_base+'/ESP8266mDNS/'])
ESP8266mDNS_lib = static_library('ESP8266mDNS', ESP8266mDNS_src, dependencies: [core, ESP8266WiFi], include_directories: ESP8266mDNS_includes)
ESP8266mDNS = declare_dependency(include_directories: ESP8266mDNS_includes, link_with: ESP8266mDNS_lib)


ArduinoOTA_src = []
foreach s: ['ArduinoOTA.cpp']
  ArduinoOTA_src += library_base+'/ArduinoOTA/'+s
endforeach
ArduinoOTA_includes = include_directories([library_base+'/ArduinoOTA/'])
ArduinoOTA_lib = static_library('ArduinoOTA', ArduinoOTA_src, dependencies: [core, ESP8266WiFi, ESP8266mDNS], include_directories: ArduinoOTA_includes)
ArduinoOTA = declare_dependency(include_directories: ArduinoOTA_includes, link_with: ArduinoOTA_lib)


ESP8266HTTPClient_src = []
foreach s: ['ESP8266HTTPClient.cpp']
  ESP8266HTTPClient_src += library_base+'/ESP8266HTTPClient/src/'+s
endforeach
ESP8266HTTPClient_includes = include_directories([library_base+'/ESP8266HTTPClient/src/'])
ESP8266HTTPClient_lib = static_library('ESP8266HTTPClient', ESP8266HTTPClient_src, dependencies: [core, ESP8266WiFi], include_directories: ESP8266HTTPClient_includes)
ESP8266HTTPClient = declare_dependency(include_directories: ESP8266HTTPClient_includes, link_with: ESP8266HTTPClient_lib)


ESP8266httpUpdate_src = []
foreach s: ['ESP8266httpUpdate.cpp']
  ESP8266httpUpdate_src += library_base+'/ESP8266httpUpdate/src/'+s
endforeach
ESP8266httpUpdate_includes = include_directories([library_base+'/ESP8266httpUpdate/src/'])
ESP8266httpUpdate_lib = static_library('ESP8266httpUpdate', ESP8266httpUpdate_src, dependencies: [core, ESP8266WiFi, ESP8266HTTPClient], include_directories: ESP8266httpUpdate_includes)
ESP8266httpUpdate = declare_dependency(include_directories: ESP8266httpUpdate_includes, link_with: ESP8266httpUpdate_lib)

