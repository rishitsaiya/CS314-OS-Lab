#!/bin/sh
./arithoh.sh &  #CPU intensive
./syscall.sh &  #Faster
wait
