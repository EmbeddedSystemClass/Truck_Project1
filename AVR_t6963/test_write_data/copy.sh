#cd ..
#rmwhitesp.sh
#cd test_write_data
clear
make clean &> out.txt
make &> out.txt
if grep -q undefined out.txt
	then cat out.txt | grep undefined
fi
if grep -q error out.txt
	then cat out.txt | grep error;exit 1
fi
make clean &>> out.txt
#cp ../eeprom/eeprom_burn.c .
tar cf test_write.tar test_write_data.c do_read.c ../eeprom/*.c  makefile ../main.h ../parse_P24.c ../main.c ../menus.c  ../t6963.* ../USART.* ../sfr_helper.h
clear
scp test_write.tar 192.168.42.115:/home/dan/dev/AVR/test_write_data
clear
scp test_write.tar 192.168.42.110:/home/dan/dev/AVR/test_write_data
rm *.tar
clear
cat out.txt | grep warning
#rm eeprom_burn.c
