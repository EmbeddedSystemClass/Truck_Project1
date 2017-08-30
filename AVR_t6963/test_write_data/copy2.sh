clear
make -f make_avr clean
tar cf sim_read.tar sim_read.c  ../eeprom_burn.c  make_avr ../main.h ../avr_main.h ../AVR_menu.c  ../t6963.* ../sfr_helper.h
clear
scp sim_read.tar 192.168.42.115:/home/dan/dev/AVR/test_write_data
clear
rm *.tar
clear
