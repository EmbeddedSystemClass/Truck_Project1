clear
rm out.txt
cd ESOS1.X
rm out.txt
make &> out.txt
cat out.txt | grep Error
cat out.txt | grep warning
cp out.txt ..
rm out.txt
#ls -ltr dist/default/production
echo ""
find . -name "*.elf"
echo ""
cd ..
