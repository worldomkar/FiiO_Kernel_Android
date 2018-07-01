# FiiO_Kernel_Android
Modded by jkbuha

Build instructions

cd

wget http://www.haoyuelectronics.com/service/RK3066/tools/linux/x86_64_arm-eabi-4.6.zip

unzip ./x86_64_arm-eabi-4.6.zip

git clone https://github.com/jkbuha/FiiO_Kernel_Android.git

cd FiiO_Kernel_Android

make clean

make ARCH=arm CROSS_COMPILE=../arm-eabi-4.6/bin/arm-eabi- rk3188_X5_defconfig

make ARCH=arm CROSS_COMPILE=../arm-eabi-4.6/bin/arm-eabi- -j8 kernel.img

mkbootimg --base 0x60400000 --pagesize 16384 --ramdisk_offset 0x62000000 --second_offset 0x60f00000 --tags_offset 0x60088000 --kernel kernel_image --ramdisk ramdisk.cpio.gz -o ./boot.img

rkflashtool w boot < boot.img
