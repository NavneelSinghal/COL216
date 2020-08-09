./a8 Assignment9/fact_registers.txt < Assignment9/fact.txt 2> status8 > /dev/null
./a9 Assignment9/fact_registers.txt < Assignment9/fact.txt 2> status9 > /dev/null

echo "checking 8 and 9"
diff status8 status9

cat status8

./a8 Assignment8/fact_registers < Assignment8/fact.txt 2> ./status8 > /dev/null
./a9 Assignment8/fact_registers < Assignment8/fact.txt 2> ./status9 > /dev/null

echo "checking 8 and 9"
diff status8 status9

cat status8
