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


Buildroot bouwen met externe toolchain (handmatig)
===
```
cd ~/el32-buildroot
make raspberrypi_defconfig
git apply /mnt/hgfs/rpi/config/br_linux_config.patch
make menuconfig
```
- Target abi: EABI (niet EABIhf)
- Build options -> Host Dir: $(HOME)/rpi_cross
- Toolchain -> Toolchain type: Buildroot toolchain
- Toolchain -> Enable large file support: yes
- Toolchain -> Enable ipv6 support: yes
- Toolchain -> Enable RPC support: yes
- Toolchain -> Enable WCHAR support: yes
- Toolchain -> Enable c++ support: yes
- Target packages -> Development tools -> binutils
- Target packages -> Hardware handling -> i2c-tools
- Target packages -> System tools -> i2c-tools
- Target packages -> System tools -> util-linux
- Target packages -> Debugging, profiling and benchmark -> gdb: yes
- Target packages -> Debugging, profiling and benchmark -> strace: yes
```
make clean && make
make menuconfig
```
- Toolchain -> Toolchain type: External toolchain
- Toolchain -> Toolchain: Custom toolchain
- Toolchain -> Toolchain path: $(HOME)/rpi_cross/usr
- Toolchain -> Toolchain has large file support: yes
- Toolchain -> Toolchain has ipv6 support: yes
- Toolchain -> Toolchain has RPC support: yes
- Toolchain -> Toolchain has WCHAR support: yes
- Toolchain -> Toolchain has threads support: yes
- Toolchain -> Toolchain has threads debugging support: no
- Toolchain -> Toolchain has c++ support: yes
- Build options -> Host dir: $(BASE_DIR)/host
```
make clean && make


Buildroot bouwen met externe toolchain (semi-automatisch)
===
```
cd ~/el32-buildroot
make raspberrypi_defconfig
git apply /mnt/hgfs/rpi/config/br_linux_config.patch
cp /mnt/hgfs/rpi/config/step1.config .config
make clean && make
cp /mnt/hgfs/rpi/config/step2.config .config
make clean && make
```