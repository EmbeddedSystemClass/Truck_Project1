tar xvf sim_read.tar
make -f make_avr clean
make -f make_avr &> out.txt
cat out.txt
sudo ./sim_read
