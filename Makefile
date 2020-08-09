all:
	g++ -o a8 ./Assignment8/simul.cpp
	g++ -o a9 ./Assignment9/simul.cpp
	g++ -o a10 ./Assignment10/simul.cpp

1:
	./a8 tests/hazard1_reg.txt < tests/hazard1.txt 2> status8 > out8
	./a9 tests/hazard1_reg.txt < tests/hazard1.txt 2> status9 > out9
	./a10 tests/hazard1_reg.txt 0.5 2 < tests/hazard1.txt 2> status10 > out10
	echo "checking 8 and 9"
	diff status8 status9
	echo "checking 8 and 10"
	diff status8 status10
	cat status8

2:
	./a8 tests/oneton_reg.txt < tests/oneton.txt 2> ./status8 > out8
	./a9 tests/oneton_reg.txt < tests/oneton.txt 2> ./status9 > out9
	./a10 tests/oneton_reg.txt 0.5 2 < tests/oneton.txt 2> ./status10 > out10
	echo "checking 8 and 9"
	diff status8 status9
	echo "checking 8 and 10"
	diff status8 status10
	cat status8

3:
	./a8 tests/acc_reg.txt < tests/acc.txt 2> status8 > out8
	./a9 tests/acc_reg.txt < tests/acc.txt 2> status9 > out9
	./a10 tests/acc_reg.txt 0.5 2 < tests/acc.txt 2> status10 > out10
	echo "checking 8 and 9"
	diff status8 status9
	echo "checking 8 and 10"
	diff status8 status10	
	cat status8


