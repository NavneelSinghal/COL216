all:
	g++ -o a8 ./Assignment8/simul.cpp
	g++ -o a9 ./Assignment9/simul.cpp
	g++ -o a10 ./Assignment10/new_simult19.cpp

1:
	./a8 Assignment9/fact_registers.txt < Assignment9/fact.txt 2> status8 > out8
	./a9 Assignment9/fact_registers.txt < Assignment9/fact.txt 2> status9 > out9
	echo "checking 8 and 9"
	diff status8 status9
	cat status8

2:
	./a8 Assignment8/fact_registers < Assignment8/fact.txt 2> ./status8 > out8
	./a9 Assignment8/fact_registers < Assignment8/fact.txt 2> ./status9 > out9
	echo "checking 8 and 9"
	diff status8 status9
	cat status8

3:
	./a8 Assignment9/acc_registers.txt < Assignment9/acc.txt 2> status8 > out8
	./a9 Assignment9/acc_registers.txt < Assignment9/acc.txt 2> status9 > out9
	echo "checking 8 and 9"
	diff status8 status9
	cat status8


