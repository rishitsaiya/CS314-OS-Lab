#!/bin/sh
./fstime.sh &  #I/O bound
./fstime.sh &  #I/O bound
./fstime.sh &  #I/O bound
wait
