cd ESOS1.X
make clean
make &> out.txt
cat out.txt | grep Error
cat out.txt | grep warning
cp out.txt ..
#ls -ltr dist/default/production
echo ""
find . -name "*.elf"
echo ""
cd ..
