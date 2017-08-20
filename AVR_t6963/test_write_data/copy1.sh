#cd ..
#rmwhitesp.sh
#cd test_write_data
clear
make -f make_pic clean &> out.txt
#cp ../eeprom/eeprom_burn.c .
tar cf sim_write.tar sim_write.c ../eeprom/eeprom_burn.c  make_pic ../avr_main.h ../PIC_menu.c  ../t6963.* ../sfr_helper.h
clear
scp sim_write.tar 192.168.42.110:/home/dan/dev/AVR/test_write_data
rm *.tar
clear
