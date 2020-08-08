g++ -o a7 ./Assignment7/sim.cpp
g++ -o a8 ./Assignment8/simul.cpp
g++ -o a9 ./Assignment9/simul.cpp
g++ -o a10 ./Assignment10/new_simult19.cpp

./a7 cycles.txt < inp 2> status7
./a8 cycles.txt < inp 2> status8
./a9 cycles.txt < inp 2> status9
./a10 cycles.txt 0.5 2 < inp 2> status10

echo "checking 7 and 8"
diff status7 status8
echo "checking 7 and 9"
diff status7 status9
echo "checking 7 and 10"
diff status7 status10

