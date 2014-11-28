rpi
=====

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
```

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

Packages voor Arch linux
===
```
sudo pacman -S rsync wget cpio python unzip bc subversion ncurses makepasswd parted dosfstools
sudo ln -s /usr/bin/makepassws /usr/bin/mkpasswd

wget https://aur.archlinux.org/packages/mu/multipath-tools-git/multipath-tools-git.tar.gz
tar -xvf multipath-tools-git.tar.gz
cd multipath-tools-git
makepkg -s
sudo pacman -U multipath-tools-git-*.pkg.tar.xz

wget https://aur.archlinux.org/packages/de/debian-whois-mkpasswd/debian-whois-mkpasswd.tar.gz
tar -xvf debian-whois-mkpasswd.tar.gz
cd debian-whois-mkpasswd-git
edit PKGBUILD to point to version 5.2.2 instead of 5.2.1
makepkg -s
sudo pacman -U debian-whois-mkpasswd-*.pkg.tar.xz
```
