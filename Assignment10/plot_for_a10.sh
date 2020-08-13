rm -f out_file

g++ -o a10 simul.cpp

for ((i=1;i<=9;i=$(($i + 1)))); do
    ./a10 tests/oneton_reg.txt 0.$i 2 < tests/oneton.txt > /dev/null 2>/dev/null
done

rm -f out_file_N

for ((i=1;i<=9;i=$(($i + 1)))); do
    ./a10 tests/oneton_reg.txt 0.7 $i < tests/oneton.txt > /dev/null 2>/dev/null
done

gnuplot plotscript.gp
gnuplot plotscript2.gp
