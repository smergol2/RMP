#!/bin/bash

cd /home/pi/workspace/

IFS=$'\n'
for i in $(ls southside); do
	ln -s "southside/$i" "$RANDOM~$i"
done

IFS=$'~\n'
x=1
rm dat
touch dat
for i in $(ls *.mp3 .); do
	if [[  $(( x%2 )) -eq 0 ]]
	then
		if [[ "$i" == *".mp3" ]]
		then
			echo "$i" >> dat
		fi
	fi
	x=$(( $x+1 ))
done
