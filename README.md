# Node-Framework: Getting Started
## Preconditions
### OS / packages
* If you have a recent/decent OS:
  * install _meson_
  * install _ninja_
* Ancient OS or No tooling for your OS? Look in doc/Prepare_meson_ninja.md

### Prepare the build system
Base this e.g. in `$HOME/prog` which we will use throughout the examples. In the following, the username running all this is _jack_ , so `$HOME/prog` expands to `/home/jack/prog`. Change this to match what you have on your system.
  1. Test your binaries/paths: 
   * `cd $HOME`
   * `meson  --version`
     * 0.44.1 (or newer, YMMV)
   * `ninja  --version`
     * 1.7.1.git (or newer, YMMV)
  1. get the _ESP8266 Arduino SDK_, but please don't name it just "Arduino":
     * `cd $HOME/prog`
     * `git clone --depth=1 https://github.com/esp8266/Arduino.git Arduino-esp8266-sdk` 
     * A directory is created automatically: `$HOME/prog/Arduino-esp8266-sdk` but that is not touched further.

### Get the sources of _node-framework_:
  * `cd $HOME/prog`
  * `git clone --depth=1 git@git.schroedingers-bit.net:home-control/modules.git`
  * Change into the new directory: `cd modules` and stay there !

### Configure _meson_ to cross-build using the _ESP8266 Arduino SDK_:
  * have or make a local configuration directory for _meson_:
    * `mkdir -p $HOME/.local/share/meson/cross/`
  * `cp $HOME/prog/modules/node-framework/esp8266.crossfile.example $HOME/.local/share/meson/cross/esp8266.crossfile` 
   * Use your preferred text editor, we call it _editor_, to edit your copy of the _crossfile_:
     * `editor $HOME/.local/share/meson/cross/esp8266.crossfile`
     * In the _crossfile_ all paths must be given absolute and there is no expansion for _$HOME_ or other variables.
     * Mostly, the path to the build tools of the _Arduino ESP8266 SDK_ (see above) must be corrected to match your system configuration.
     * Example _esp8266.crossfile_, if you followed the instructions above:

``` plain
; Copy this to ~/.local/share/meson/cross/
[binaries]
c = '/home/jack/prog/Arduino-esp8266-sdk/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc'
cpp = '/home/jack/prog/Arduino-esp8266-sdk/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-g++'
ar = '/home/jack/prog/Arduino-esp8266-sdk/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-ar'
strip = '/home/jack/prog/Arduino-esp8266-sdk/tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-strip'

[properties]
esp8266_base = '/home/jack/prog/Arduino-esp8266-sdk/'
esptool = '/home/jack/prog/Arduino-esp8266-sdk/tools/esptool/esptool'

[host_machine]
system = 'bare'
cpu_family = 'xtensa'
cpu = 'lx106'
endian = 'none'
```

### Configure cross-compiling _meson_  for the _node-framework_:
  * `cd $HOME/prog/modules/node-framework`
  * Verify the _meson.build_ file: `ls -l meson.build`
  * Build a configuration: `meson --cross-file=esp8266.crossfile build`

### Test build the existing _node-framework_ projects using _ninja_:
  * `cd $HOME/prog/modules/node-framework`
  * Prepare a build directory:
    * `mkdir build`
  * `ninja -C build`
    * This should complete without error.

  ## Roll your own project:
  * Example: an (empty) Arduino project, based on the ESP8266 Arduino SPISlave_Test [^1].
[^1]: https://github.com/esp8266/Arduino/blob/master/libraries/SPISlave/examples/SPISlave_Test/SPISlave_Test.ino
 * We base this project's source file names on __spi-slave-test__ and call it __spi_slave_test__ in the build environment.
  ### Create the source code:
 * `cd $HOME/prog/modules/node-framework`
 * Create the source: `cat >> spi-slave-test.cpp`

``` c++
#include <Arduino.h>
void setup()
{
    //node.setup();
}

void loop()
{
    //node.loop();
}
```

### Add the project to the _meson.build_ file:
 * This project will use SPI which we pull from Arduino's pre-configured libraries using _arduino.get_variable('SPISlave')_ (TODO: Use SPISlave, not, SPI, working on it ...)
 * `cd $HOME/prog/modules/node-framework`
 * `cat >> meson.build`

``` python3
spi_slave_test_exe = executable('spi_slave_test.elf', ['spi-slave-test.cpp', ], dependencies: [
  sdk.get_variable('core'),
  arduino.get_variable('SPI'),
  ] )
spi_slave_test_bin = custom_target('spi_slave_test.bin',
  input : spi_slave_test_exe,
  output : 'spi_slave_test.bin',
  command : sdk.get_variable('esptoolize'),
  build_by_default: true,
)
```

#### Add the _SPISlave_  library from the _ESP8266 Arduino SDK_  to the _meson_ configuration:
  * `editor subprojects/esp8266_arduino/meson.build`
  * :TODO:

### Explanation of cross-compilation process:
* Sources -> ELF binary -> stripping -> .bin image extraction -> ESP8266 styling (esptoolize)

### Build it!
 * `cd $HOME/prog/modules/node-framework`
 * `ninja -C build`

