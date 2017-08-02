gcc -DMAIN -Wall demo_forms4.c form_supplement.c  -o demo4  -lform -lcurses &> output2.txt
echo "warning output2.txt" >> output2.txt
cat output2.txt | grep warning
cat output2.txt | grep error
