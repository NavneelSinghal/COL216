g++ -o a8 ./Assignment8/simul.cpp
g++ -o a9 ./Assignment9/simul.cpp
g++ -o a10 ./Assignment10/new_simult19.cpp

./a8 cycles.txt < inp > /dev/null 2> status8
./a9 cycles.txt < inp > /dev/null 2> status9
./a10 cycles.txt 0.5 2 < inp > /dev/null 2> status10

echo "checking 8 and 9"
diff status8 status9
echo "checking 8 and 10"
diff status8 status10

