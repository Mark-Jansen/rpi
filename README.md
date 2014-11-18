rpi
===

Directory structure:

- application - the main folder for the application
- build - output build folder
- config - contains a patch file to be applied to the buildroot (to enable i2c)
- datasheets - contains datasheets for hardware components used
- device - optional scripts to use on the rpi
- modules - the main folder for all kernel modules
- scripts - helper scripts
- test - test applications

Buildroot used
===
https://github.com/fhict/el32-buildroot


Buildroot bouwen met externe toolchain
===
```
cd ~/el32-buildroot
make menuconfig
```
- Target abi: EABI (niet EABIHF)
- Build options -> Host Dir: $(HOME)/rpi_cross
- Toolchain -> Toolchain type: Buildroot toolchain
- Toolchain -> Enable c++ support: yes
```
make clean && make
make menuconfig
```
- Toolchain -> Toolchain type: External toolchain
- Toolchain -> Toolchain path: $(HOME)/rpi_cross/usr
- Toolchain -> Toolchain has c++ support: yes
```
make clean && make
