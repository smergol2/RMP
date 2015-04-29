#!/bin/bash

cd /home/pi/workspace/

read -r l < curDir

IFS=$'\n'
for i in $(ls $l); do
	ln -s "$l/$i" "$RANDOM~$i"
	echo $l/$i
done

IFS=$'~\n'
x=1
rm dat
touch dat
for i in $(ls *.mp3 .); do
	if [[  $(( x%2 )) -eq 0 ]]
	then
		if [[ "$i" == *"."* ]]
		then
			echo "$i" >> dat
		fi
	fi
	x=$(( $x+1 ))
done
