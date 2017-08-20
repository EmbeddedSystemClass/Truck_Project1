clear
make -f make_avr clean &> out.txt
tar cf sim_read.tar sim_read.c  ../eeprom/eeprom_burn2.c  make_avr ../avr_main.h ../AVR_menu.c  ../t6963.* ../sfr_helper.h
clear
scp sim_read.tar 192.168.42.115:/home/dan/dev/AVR/test_write_data
clear
rm *.tar
clear
cat out.txt | grep warning
#rm eeprom_burn.c
