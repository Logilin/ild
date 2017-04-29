#! /bin/bash

DEVICE=/dev/velleman0


while true; do
	if [ ! -c ${DEVICE} ]
	then
		exit 1
	fi
	for i in $(seq 0 2 255)
	do
#		analog1=$(($RANDOM & 0xFF))
#		analog2=$(($RANDOM & 0xFF))
#		digital=$(($RANDOM & 0xFF))
		if ! echo 5 $i $i $i 0 0 0 0 
		then
			exit 1
		fi
#		usleep 10000
#		sleep 0.01
	done
done > $DEVICE

