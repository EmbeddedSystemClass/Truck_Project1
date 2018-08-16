# ultimate script for compiling and flashing an AVR
date
make clean
make &> out.txt
if grep -q error out.txt
 find2 error out.txt
then exit
fi
echo "passed"
make flash &> out2.txt
cat out2.txt | tail -n21
make clean
date
