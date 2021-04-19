#!/bin/sh
./arithoh.sh & #CPU intensive
./fstime.sh &  #I/O bound
wait
