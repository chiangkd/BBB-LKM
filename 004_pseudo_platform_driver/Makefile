TARGET_MODULE := pseudo_device_setup
TARGET_MODULE2 := pseudo_driver
obj-m := $(TARGET_MODULE).o $(TARGET_MODULE2).o

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
	scp $(TARGET_MODULE2).ko debian@192.168.7.2:/home/debian/drivers


# Client part

CC := arm-linux-gnueabihf-gcc
TARGET_CLIENT := client
client:
	$(CC) -o $(TARGET_CLIENT) client.c

upload-client:
	scp $(TARGET_CLIENT).c debian@192.168.7.2:/home/debian/drivers

clean-client:
	rm -rf client