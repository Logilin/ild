#! /bin/bash

DEVICE=/dev/velleman0

if [ ! -c ${DEVICE} ]
then
	exit 1
fi

while true
do
	read digital_in unused analog1_in analog2_in rest < $DEVICE
	analog1_out=$((analog1_in))
	analog2_out=$((analog2_in))
	digital_out=$((digital_in))
	echo 5 $digital_out $analog1_out $analog2_out 0 0 0 0 > $DEVICE
	if [ $? -ne 0 ]; then break ; fi
done

