# Preparing _meson_ and _ninja_
### Ubuntu 14.04 LTS -- for a non cutting edge system.
* Packages: 
  * :TODO: (using a dev-machine with many dev-packages installed, no clue what is actually required)

### Prepare the build system
Base this e.g. in `$HOME/prog` which we will use throughout the examples. In the following, the username running all this is _jack_ , so `$HOME/prog` expands to `/home/jack/prog`. Change this to match what you have on your system.

1. _ninja_: 
   * `cd $HOME/prog`
   * `git clone --depth=1 https://github.com/ninja-build/ninja.git`
   *  commit 94fc1431 (ca. 2016-07-31) works
   * A directory is created automatically, go there: `cd $HOME/prog/ninja`
   * Build as _ninja_ documentation tells: `./configure.py --bootstrap`
2.  _meson_: 
    * `cd $HOME/prog`
    * With Debian/Ubuntu stock Python 3.4, _meson_ 0.44.1 is the latest working version:
      * `git clone --depth=1 --branch 0.44.1 https://github.com/mesonbuild/meson.git`
    * With Python >3.4, use the latest _meson_:
      * `git clone --depth=1 https://github.com/mesonbuild/meson.git`
    * A directory is created automatically: `$HOME/prog/meson` but that is not touched further.
4. Putting the binaries on the `$PATH`
   * If you have a custom `bin`directory on your path, use that. For example, `$HOME/bin` is a common place.
   * Make sure this directory is listed early in the output of `echo $PATH`
   * If you don't have a custom `bin`directory, any other (early) directory in `echo $PATH` will do, e.g. `/usr/local/bin` could be the place.
   * Make Symlinks for _meson_ and _ninja_
     1. `cd $HOME/bin`or `cd /usr/local/bin`, depending on what you have 
     1. `ln -s $HOME/prog/meson/meson.py meson`
     1. `ln -s $HOME/prog/ninja/ninja ninja`
5. Test your binaries/paths: 
   * `cd $HOME`
   * `meson  --version`
     * 0.44.1 (or newer, YMMV)
   * `ninja  --version`
     * 1.7.1.git (or newer, YMMV)
