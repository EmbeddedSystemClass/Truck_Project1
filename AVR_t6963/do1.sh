# ultimate script for compiling and flashing an AVR
date
make clean
make &> out.txt
if grep -q error out.txt
 find2 error out.txt
then exit
fi
echo "passed the compile portion"
make flash &> out2.txt
echo "now burning program to AVR"
cat out2.txt | tail -n21
make clean
echo "done"
date
