#cd ..
#rmwhitesp.sh
#cd test_write_data
make clean
#cp ../eeprom/eeprom_burn.c .
tar cf test_write.tar test_write_data.c do_read.c ../eeprom/eeprom_burn.c  makefile ../main.h ../main.c ../menus.c ../parse_P24.c ../t6963.* ../USART.* ../sfr_helper.h
scp test_write.tar 192.168.42.115:/home/dan/dev/AVR/test_write_data
scp test_write.tar 192.168.42.110:/home/dan/dev/AVR/test_write_data
rm *.tar
#rm eeprom_burn.c
