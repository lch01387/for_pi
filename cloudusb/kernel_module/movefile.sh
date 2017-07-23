#!/bin/bash
cp /lib/modules/4.4.50+/source/drivers/usb/gadget/function/f_mass_storage.c /lib/modules/4.4.50+/source/drivers/usb/gadget/function/f_mass_storage.c.backup
cp /lib/modules/4.4.50+/source/drivers/usb/gadget/function/Makefile /lib/modules/4.4.50+/source/drivers/usb/gadget/function/Makefile.backup
cp f_mass_storage.c /lib/modules/4.4.50+/source/drivers/usb/gadget/function/f_mass_storage.c
cp Makefile /lib/modules/4.4.50+/source/drivers/usb/gadget/Makefile
cp compile.sh /lib/modules/4.4.50+/source/compile.sh
