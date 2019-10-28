make -C /home/mannu/Downloads/linux-4.6.3-copy/ M=$PWD
rmmod fourMbDriver
insmod fourMbDriver.ko
rm /dev/fourMbDriver
mknod /dev/fourMbDriver c 62 0
