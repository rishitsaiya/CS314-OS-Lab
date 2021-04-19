#Gnuplot script for plotting data in file "random.dat"
set terminal png
set output "optimal.png"

set title "Page replacement using optimal policy"

set key right center

set xlabel "frms in memory"
set ylabel "hit ratio"

set xrange [0:100]
set yrange [0:1]

plot "optimal.dat" u 1:2 w linespoints title "optimal"
