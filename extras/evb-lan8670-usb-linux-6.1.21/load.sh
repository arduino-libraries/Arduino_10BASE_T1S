#!/bin/bash

enable=1
node_id=4
node_count=8
max_bc=0
burst_timer=128
to_timer=32

if [[ $1 == "" ]]
then
	echo Proceeding with node id: 0
else
	node_id=$1
fi

if [ "$node_id" -ge 0 ] && [ "$node_id" -le 254 ]
then
	echo PLCA Parameters:
	echo ----------------
	echo PLCA: $enable
	echo Node id: $node_id
	echo Node count: $node_count
	echo Max BC: $max_bc
	echo Burst Timer: $burst_timer
	echo TO Timer: $to_timer
	echo ----------------
else
	echo Invalid node id: "$node_id"
	exit 0
fi

devices=$(ls /sys/bus/usb/drivers/smsc95xx | grep "^[1-9]")
for i in $devices; do
	echo Unlinking device: "$i"
	echo "$i" | sudo tee /sys/bus/usb/drivers/smsc95xx/unbind > /dev/null
done

if [[ -n $(lsmod | grep microchip_t1s) ]]
then
	echo Unloading microchip_t1s driver...
	sudo rmmod microchip_t1s
fi

echo Loading microchip_t1s driver...
sudo insmod microchip_t1s.ko enable=$enable node_id=$node_id node_count=$node_count max_bc=$max_bc burst_timer=$burst_timer to_timer=$to_timer

for i in $devices; do
	echo Linking device: "$i"
        echo "$i" | sudo tee /sys/bus/usb/drivers/smsc95xx/bind > /dev/null
done
