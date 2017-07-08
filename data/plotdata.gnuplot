set size square

set ylabel "Framewise variance"
set xlabel "Frame number"

set term svg
set output "gnuplotoutput.svg"
plot "gnuplotinput.txt" using 1:2 with lines
