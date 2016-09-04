project('esp8266_sdk', ['c', 'cpp'])

esp8266_base = meson.get_cross_property('esp8266_base')
esptool = find_program(meson.get_cross_property('esptool'))

esp8266_include=include_directories([esp8266_base+'/cores/esp8266', esp8266_base+'/variants/generic',
esp8266_base+'/tools/sdk/include', esp8266_base+'/tools/sdk/lwip/include'])
esp8266_src = []
foreach s: ['spiffs_hal.cpp', 'libc_replacements.c', 'Tone.cpp', 'uart.c', 'cbuf.cpp', 'cont_util.c', 'spiffs_api.cpp', 'core_esp8266_wiring_digital.c', 'cont.S', 'base64.cpp', 'debug.cpp', 'core_esp8266_si2c.c', 'time.c', 'Stream.cpp', 'StreamString.cpp', 'core_esp8266_wiring_shift.c', 'spiffs/spiffs_check.c', 'spiffs/spiffs_hydrogen.c', 'spiffs/spiffs_gc.c', 'spiffs/spiffs_cache.c', 'spiffs/spiffs_nucleus.c', 'MD5Builder.cpp', 'abi.cpp', 'core_esp8266_wiring.c', 'HardwareSerial.cpp', 'core_esp8266_timer.c', 'core_esp8266_postmortem.c', 'heap.c', 'pgmspace.cpp', 'core_esp8266_eboot_command.c', 'core_esp8266_wiring_pulse.c', 'core_esp8266_wiring_analog.c', 'core_esp8266_flash_utils.c', 'libb64/cdecode.c', 'libb64/cencode.c', 'WString.cpp', 'Esp.cpp', 'Updater.cpp', 'WMath.cpp', 'core_esp8266_phy.c', 'Print.cpp', 'umm_malloc/umm_malloc.c', 'core_esp8266_main.cpp', 'FS.cpp', 'Schedule.cpp', 'core_esp8266_i2s.c', 'core_esp8266_wiring_pwm.c', 'core_esp8266_noniso.c', 'IPAddress.cpp', 'WString.cpp']
  esp8266_src += esp8266_base+'/cores/esp8266/'+s
endforeach

core_lib = static_library('ESP8266Core', esp8266_src, include_directories: esp8266_include)
core=declare_dependency(include_directories: esp8266_include,
link_args: ['-L/home/trilader/code/esp8266-sdk/tools/sdk/lib', '-L/home/trilader/code/esp8266-sdk/tools/sdk/ld', '-lm', '-lgcc', '-lhal', '-lphy', '-lnet80211', '-llwip_gcc', '-lmain', '-lpp', '-lsmartconfig', '-lwps', '-lcrypto', '-laxtls', '-lespnow', '-laxtls', '-lmesh', '-lwpa', '-lwpa2', 
'subprojects/esp8266_sdk/libESP8266Core.a', # this is an ugly hack. fix it
'-lstdc++','-lc', '-lg'],
link_with: core_lib)

esptoolize=[esptool,
  '-eo', esp8266_base+'/bootloaders/eboot/eboot.elf',
  '-bo', '@OUTPUT@',
  '-bm', 'dio',
  '-bf', '40',
  '-bz', '4M',
  '-bs', '.text',
  '-bp', '4096',
  '-ec',
  '-eo', '@INPUT@',
  '-bs', '.irom0.text',
  '-bs', '.text',
  '-bs', '.data',
  '-bs', '.rodata',
  '-bc',
  '-ec']



