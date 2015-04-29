#!/bin/bash
rm ../curDir
touch ../curDir
echo /home/pi/workspace/Schlager > ../curDir

sudo mount -t vfat -o utf8,uid=pi,gid=pi,noatime /dev/sda1 /media/usbstick
sleep 1

/home/pi/workspace/skripte/play.sh
