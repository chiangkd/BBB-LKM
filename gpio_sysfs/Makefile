TARGET_MODULE := gpio-sysfs
obj-m := $(TARGET_MODULE).o

HEADER := 5.10.168-ti-r71

KDIR := /lib/modules/$(HEADER)/build
PWD := $(shell pwd)
ARCH := arm
CROSS_COMPILE := arm-linux-gnueabihf-

HOSTKDIR := /lib/modules/$(shell uname -r)/build

# Device Tree
DTC ?= dtc

all:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean

host:
	$(MAKE) -C $(HOSTKDIR) M=$(PWD) modules

test-overlay:
	fdtoverlay -i am335x-boneblack-uboot-univ.dtb -o merged.dtb pcdev1.dtbo

upload:
	scp $(TARGET_MODULE).ko debian@192.168.7.2:/home/debian/drivers

upload-dtb:
# 	scp am335x-boneblack.dtb debian@192.168.7.2:/home/debian/drivers
	scp am335x-boneblack-uboot-univ.dtb debian@192.168.7.2:/home/debian/drivers

upload-dtbo:
	scp pcdev1.dtbo debian@192.168.7.2:/home/debian/drivers

download-dts:
	scp debian@192.168.7.2:/home/debian/drivers/am335x-boneblack-uboot-univ.dts ./

clean-client:
	rm -rf client