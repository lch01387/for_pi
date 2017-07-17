make modules SUBDIRS=drivers/usb/gadget/function
sudo make modules_install SUBDIRS=./drivers/usb/gadget/function
cp /lib/modules/4.4.50+/source/drivers/usb/gadget/function/usb_f_mass_storage.ko /lib/modules/4.4.50+/build/drivers/usb/gadget/function/usb_f_mass_storage.ko
