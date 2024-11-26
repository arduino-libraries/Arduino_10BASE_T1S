`Arduino_10BASE_T1S`
====================
[![Compile Examples](https://github.com/bcmi-labs/Arduino_10BASE_T1S/workflows/Compile%20Examples/badge.svg)](https://github.com/bcmi-labs/Arduino_10BASE_T1S/actions?workflow=Compile+Examples)
[![Spell Check status](https://github.com/bcmi-labs/Arduino_10BASE_T1S/actions/workflows/spell-check-task.yml/badge.svg)](https://github.com/bcmi-labs/Arduino_10BASE_T1S/actions/workflows/spell-check-task.yml)
[![Sync Labels status](https://github.com/bcmi-labs/Arduino_10BASE_T1S/actions/workflows/sync-labels.yml/badge.svg)](https://github.com/bcmi-labs/Arduino_10BASE_T1S/actions/workflows/sync-labels.yml)
[![Arduino Lint](https://github.com/bcmi-labs/Arduino_10BASE_T1S/workflows/Arduino%20Lint/badge.svg)](https://github.com/bcmi-labs/Arduino_10BASE_T1S/actions?workflow=Arduino+Lint)

**Note**: This library works for Arduino [Zero](https://store.arduino.cc/products/arduino-zero), Arduino [R4 WiFi](https://store.arduino.cc/products/uno-r4-wifi) and Arduino [R4 Minima](https://store.arduino.cc/products/uno-r4-minima).

### How-to-compile/upload
```bash
arduino-cli compile -b arduino:renesas_uno:unor4wifi -v examples/UDP_Client -u -p /dev/ttyACM0
```

### How-to-[`EVB-LAN8670-USB`](https://www.microchip.com/en-us/development-tool/EV08L38A)
**Note**: Keep [this application note](https://microchip.my.site.com/s/article/EVB-LAN8670-USB-Enablement-for-Debian-Ubuntu-Raspbian) in mind when building for Debian based systems. You may need to revert back to legacy network configuration tools.

* Disconnect `EVB-LAN8670-USB`, if it is already connected.
* Build kernel driver:
```bash
cd extras/evb-lan8670-usb-linux-6.1.21
make
```
* Load kernel driver:
```bash
sudo insmod microchip_t1s.ko enable=1 node_id=0 node_count=8 max_bc=0 burst_timer=128 to_timer=32
```
* `dmesg` output when connecting the USB dongle:
```bash
[  +0,124736] usb 1-2.3: New USB device found, idVendor=184f, idProduct=0051, bcdDevice= 2.00
[  +0,000016] usb 1-2.3: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[  +0,000006] usb 1-2.3: Product: 10BASE-T1S
[  +0,000005] usb 1-2.3: Manufacturer: MCHP
[  +0,000005] usb 1-2.3: SerialNumber: 0000465
[  +0,004338] smsc95xx v2.0.0
[  +0,582091] LAN867X Rev.B1 usb-001:025:00: PLCA mode enabled. Node Id: 0, Node Count: 8, Max BC: 0, Burst Timer: 128, TO Timer: 32
[  +0,000176] LAN867X Rev.B1 usb-001:025:00: attached PHY driver (mii_bus:phy_addr=usb-001:025:00, irq=190)
[  +0,000285] smsc95xx 1-2.3:1.0 eth2: register 'smsc95xx' at usb-0000:00:14.0-2.3, smsc95xx USB 2.0 Ethernet, 00:1e:c0:d1:b9:4b
```
* Configure IP address for `eth1`: (*Note*: it could also be another `eth` interface, you need to check and compare MAC address against `dmesg` output). 
```bash
sudo ip addr add dev eth1 192.168.42.100/24
```
* You can take a look at the registered Ethernet device via `ip link show eth1`:
```bash
3: eth1: <NO-CARRIER,BROADCAST,MULTICAST,UP> mtu 1500 qdisc fq_codel state DOWN group default qlen 1000
    link/ether 3c:e1:a1:b8:e9:76 brd ff:ff:ff:ff:ff:ff
    inet 192.168.42.100/24 scope global eth1
       valid_lft forever preferred_lft forever
```
* Bring `eth1` interface up via `sudo ifconfig eth1 up`.
* Verify `eth1` via `ifconfig eth1`:
```bash
eth1: flags=4099<UP,BROADCAST,MULTICAST>  mtu 1500
        inet 192.168.42.100  netmask 255.255.255.0  broadcast 0.0.0.0
        ether 3c:e1:a1:b8:e9:76  txqueuelen 1000  (Ethernet)
        RX packets 0  bytes 0 (0.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 0  bytes 0 (0.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0
```

### How-to-`tcpdump`
```bash
tcpdump -i eth1
```
