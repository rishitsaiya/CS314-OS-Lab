#!/bin/bash
echo "Enter 1 for random
Enter 2 for optimal
Enter 3 for lru
Enter 4 for clock"
read var
if [ var == "1" ]; then
    gcc random.c -o random 
    3./random > random.dat 
    gnuplot random.p
elif [ var == "2" ]; then
    gcc optimal.c -o optimal 
    ./optimal > optimal.dat 
    gnuplot optimal.p
elif [ var == "3" ]; then
    gcc lru.c -o lru
    ./lru > lru.dat
    gnuplot lru.p
elif [ var == "4" ]; then
    gcc clock.c -o clock
    ./clock > clock.dat
    gnuplot clock.p
fi