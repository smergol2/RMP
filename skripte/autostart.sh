#!/bin/bash
rm ../curDir
touch ../curDir
echo /home/pi/workspace/RMP/Schlager > ../curDir

sudo mount -t vfat -o utf8,uid=pi,gid=pi,noatime /dev/sda1 /media/usbstick
sleep 1

/home/pi/workspace/RMP/skripte/play.sh
