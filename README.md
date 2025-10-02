# BBB-LKM

BBB-LKM is an experimental project to learn kernel module development. Build kernel object on host machine (x86 arch.) and then upload the program to the device (BeagleBone black) through secure copy (`scp`) program

## Prerequisite

A USB to TTL module and `minicom` for debugging

### Image and library
1. Download Image from [official website](https://www.beagleboard.org/distros/am335x-11-7-2023-09-02-4gb-microsd-iot) (Kernel version: 5.10.168-ti-r71)
2. Use [ti-linux-kernel-dev](https://github.com/RobertCNelson/ti-linux-kernel-dev) to generate kernel header

Use `-bone` version might cannot find related `linux-kernel-headers-`

Build kernel header (in `ti-linux-kernel-dev`)
```shell
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-
./build_kernel.sh
cd KERNEL
make prepare
make modules_prepare
```

Create symbolic link
```shell
sudo mkdir -p /lib/modules/5.10.168-ti-r71
sudo ln -sf $(pwd)/KERNEL /lib/modules/5.10.168-ti-r71/build
```

### Compile and upload kernel module

The simplest `makefile` in each example is as below
```shell
TARGET_MODULE := pseudo
obj-m := $(TARGET_MODULE).o

HEADER := 5.10.168-ti-r71

KDIR := /lib/modules/$(HEADER)/build
PWD := $(shell pwd)
ARCH := arm
CROSS_COMPILE := arm-linux-gnueabihf-

HOSTKDIR := /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(PWD) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

upload:
	scp $(TARGET_MODULE).ko debian@192.168.7.2:/home/debian/drivers
```
- `make all` can bulild `{TARGET_MODULE}.ko`
- `make upload` can upload `{TARGET_MODULE}.ko` from host to BBB (`/home/debian/drivers`)

## Enable Interet on BBB
1. BBB command
```shell
sudo route add default gw 192.168.7.1
```

2. HOST command
```shell
# 1. Enable NAT
sudo iptables -t nat -A POSTROUTING -o wlo1 -j MASQUERADE

# 2. Packet forwarding
sudo iptables -A FORWARD -i enx880ce043dc73 -o wlo1 -j ACCEPT
sudo iptables -A FORWARD -i wlo1 -o enx880ce043dc73 -m state --state RELATED,ESTABLISHED -j ACCEPT

# 3. Enable packet forwarding
echo 1 | sudo tee /proc/sys/net/ipv4/ip_forward
```

## Reference

- Udemy Courses
    - [Linux Device Driver Programming With Beaglebone Black (LDD1)](https://www.udemy.com/course/linux-device-driver-programming-using-beaglebone-black/?couponCode=PMNVD2025)
    - [Embedded Linux Step by Step Using Beaglebone Black](https://www.udemy.com/course/embedded-linux-step-by-step-using-beaglebone/?couponCode=PMNVD2025)
- [The Linux Kernel Module Programming Guide](https://github.com/sysprog21/lkmpg)
