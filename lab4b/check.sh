#!/bin/bash
 
{ echo "SCALE=F"; sleep 2; echo "SCALE=C"; sleep 3; echo "OFF"; } | ./lab4b --log log_make_check.txt 

if [ $? -ne 0 ]
then
	echo "exited with nonzero status"
else
	echo "success"
fi


grep "SHUTDOWN" log_make_check.txt

if [ $? -ne 0 ]
then
	echo "shutdown not logged"
else
        echo "success logging shutdown"
fi

rm -f log_make_check.txt
