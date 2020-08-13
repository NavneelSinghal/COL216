set size ratio 1
set terminal png
set output 'out2.png'
set xlabel 'Penalty cycles'
set ylabel 'Number of clock cycles'
plot 'out_file_N' ls 4 notitle
exit
