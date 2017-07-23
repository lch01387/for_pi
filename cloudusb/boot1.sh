#!/bin/bash

sh /home/pi/for_pi/cloudusb/communicate_module/makenode.sh
sudo insmod /home/pi/for_pi/cloudusb/communicate_module/connModule.ko
/home/pi/cloudeusb/MYFAT/MYFAT/MYFAT/main.out &
