clear
tar xvf sim_read.tar
make -f make_avr clean
mv *.c *.h ..
mv ../sim_read.c .
make -f make_avr  &> out.txt
rm *.o
mv *.tar temp
sudo ./sim_read
