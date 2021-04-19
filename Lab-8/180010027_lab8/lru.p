#Gnuplot script for plotting data in file "lru.dat"
set terminal png
set output "lru.png"

set title "Page replacement using LRU policy"

set key right center

set xlabel "frms in memory"
set ylabel "hit ratio"

set xrange [0:100]
set yrange [0:1]

plot "lru.dat" u 1:2 w linespoints title "LRU"
