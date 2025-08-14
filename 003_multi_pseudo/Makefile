TARGET_MODULE := pseudo_n
obj-m := $(TARGET_MODULE).o

HEADER := 5.10.168-ti-r71

KDIR := /lib/modules/$(HEADER)/build
PWD := $(shell pwd)
ARCH := arm
CROSS_COMPILE := arm-linux-gnueabihf-

HOSTKDIR := /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean

help:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) help

host:
	$(MAKE) -C $(HOSTKDIR) M=$(PWD) modules

upload:
	scp $(TARGET_MODULE).ko debian@192.168.7.2:/home/debian/drivers