assert(meson.is_cross_build(), 'ESP8266 projects can only be built in a cross build environment.')

board_cross_args = {
    'nodemcu' : [
        '-DARDUINO_ESP8266_NODEMCU',
    ],
    'sonoff' : [
        '-DARDUINO_ESP8266_ESP01',
    ],
    'shelly' : [
        '-DARDUINO_ESP8266_GENERIC',
    ],
}

common_cross_args = [
    '-D__ets__',
    '-DICACHE_FLASH',
    '-U__STRICT_ANSI__',
    '-DF_CPU=80000000L',
    '-DARDUINO=10805',
    '-DARDUINO_ARCH_ESP8266',
    '-DESP8266',
    '-g',
    '-mlongcalls',
    '-mtext-section-literals',
    '-falign-functions=4',
    '-MMD',
    '-ffunction-sections',
    '-fdata-sections',
    '-DLWIP_OPEN_SRC',
    '-DTCP_MSS=1460',
] + board_cross_args[get_option('boardtype')]

add_global_arguments(common_cross_args + [
    '-x assembler-with-cpp',
    ], language: 'asm')

add_global_arguments(common_cross_args + [
    '-Wpointer-arith',
    '-Wno-implicit-function-declaration',
    '-Wl,-EL',
    '-fno-inline-functions',
    '-nostdlib',
    '-std=gnu99',
    ], language: 'c')

add_global_arguments(common_cross_args + [
    '-fno-exceptions',
    '-fno-rtti',
    '-std=c++11',
    '-DVTABLES_IN_FLASH',
    ], language: 'cpp')

board_cross_link_args = {
    'nodemcu' : [
        '-Teagle.flash.4m.ld',
    ],
    'sonoff' : [
        '-Teagle.flash.1m0.ld',
    ],
    'shelly' : [
        '-Teagle.flash.4m.ld',
    ],
}

common_cross_link_args = [
    '-g',
    '-w',
    '-nostdlib',
    '-Wl,--no-check-sections',
    '-u", "app_entry',
    '-Wl,-static',
    '-Wl,--gc-sections',
    '-Wl,-wrap,system_restart_local',
    '-Wl,-wrap,spi_flash_read',
] + board_cross_link_args[get_option('boardtype')]

add_global_link_arguments(common_cross_link_args + ['-Wl,-wrap,__register_chipv6_phy'], language: 'c')
add_global_link_arguments(common_cross_link_args, language: 'cpp')
