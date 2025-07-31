# BBB-LKM

Linux kernel module runs on BeagleBone Black

## Prerequisite

USB to TTL module and `minicom` for debugging

## Image
1. Download Image from [official website](https://www.beagleboard.org/distros/am335x-11-7-2023-09-02-4gb-microsd-iot)
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
- Check there is `Module.symvers`

Create symbolic link
```shell
sudo mkdir -p /lib/modules/5.10.168-ti-r71
sudo ln -sf $(pwd)/KERNEL /lib/modules/5.10.168-ti-r71/build
```

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
