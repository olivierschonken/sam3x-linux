sam3x (cortex-m3) linux
================
Linux source with SAM3X (cortex-m3) board support added

Make ARCH=arm at91sam3x8h_defconfig

make ARCH=arm menuconfig to select additional options

make ARCH=arm CROSS_COMPILE=arm-none-linux-gnueabi- Image

kernel image can be found in arch/arm/boot/Image


