clear
make -f make_avr clean
tar cf sim_read.tar sim_read2.c  make_avr
clear
scp sim_read.tar 192.168.42.115:/home/dan/dev/sim_read
clear
rm *.tar
clear
