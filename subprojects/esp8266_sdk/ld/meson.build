script = find_program('customize_sdk.py')

customize_sdk_4m = custom_target('Generate Linker Script 4m',
    input: esp8266_base+'/tools/sdk/ld/eagle.flash.4m.ld',
    output: 'eagle.flash.4m.ld',
    depfile: 'ld.deps',
    command: [script, '@INPUT@', '@OUTPUT@', '@DEPFILE@', find_program('c', required: true, native: false)]
)

customize_sdk_1m0 = custom_target('Generate Linker Script 1m0',
    input: esp8266_base+'/tools/sdk/ld/eagle.flash.1m0.ld',
    output: 'eagle.flash.1m0.ld',
    depfile: 'ld.deps',
    command: [script, '@INPUT@', '@OUTPUT@', '@DEPFILE@', find_program('c', required: true, native: false)]
)

customize_sdk_1m64 = custom_target('Generate Linker Script 1m64',
    input: esp8266_base+'/tools/sdk/ld/eagle.flash.1m64.ld',
    output: 'eagle.flash.1m64.ld',
    depfile: 'ld.deps',
    command: [script, '@INPUT@', '@OUTPUT@', '@DEPFILE@', find_program('c', required: true, native: false)]
)

sdk_ld_dir = meson.current_build_dir()
