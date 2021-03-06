#!/bin/bash
sleep 5
cd /home/pi/workspace/RMP/skripte
#pwd

sudo ../RaspiLCD/raspilcd &
#sleep 10
#shuffling music
rm ../*.*
./shuffle.sh
alsaplayer -P ../*.* &
sleep 1
alsaplayer --volume 1.0
#alsaplayer -P
sleep 1


#play music
IFS=$'/~'
while true
do
	#get title
	alsaplayer --status > ../playlist
	IFS=$'~'
	while read -r line
	do
		path='path:'
		if [[ "$line" == "$path"* ]]
		then
			curr=($line)
			title="${curr[1]}"
			echo "${curr[1]}" > ../currentTitle
		fi
	done < ../playlist

	IFS=$'/~'
	#bedienung
	read -r l < ../steuer
	tmp=($l)
	
	#play
	start='--start'
	if [[ "$tmp" == "$start"* ]]
	then
		alsaplayer --start
	fi

	#pause
	pause='--pause'
	if [[ "$tmp" == "$pause"* ]]
	then
		alsaplayer --pause
	fi

	#previous song
	prev='--prev'
	if [[ "$tmp" == "$prev"* ]]
	then
		alsaplayer --prev
	fi
	
	#next song
	next='--next'
	if [[ "$tmp" == "$next"* ]]
	then
		alsaplayer --next
	fi
	
	#specific song
	read -r l2 < ../song
	newSongNum=($l2)
	if [[ $newSongNum -gt 0 ]]
	then
		alsaplayer --jump $newSongNum
	fi

	#lauter/leiser

        read -r l3 < ../play
        if [[ $l3 -gt 0 ]]
        then
		alsaplayer --quit
                rm ../*.*
                ../skripte/shuffle.sh
                alsaplayer -P ../*.* &
                sleep 1
                alsaplayer --volume 1.0
        fi
	
done
