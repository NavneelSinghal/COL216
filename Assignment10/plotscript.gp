set size ratio 1
set terminal png
set output 'out.png'
set xlabel 'Probability'
set ylabel 'Number of clock cycles'
plot 'out_file' ls 4 notitle
exit
