#cd ..
#rmwhitesp.sh
#cd test_write_data
clear
make -f make_pic clean
tar cf sim_write.tar sim_write.c make_pic ../pic_main.h ../main.h ../PIC_menu.c  ../t6963.* ../sfr_helper.h  ../supp.c
clear
scp sim_write.tar 192.168.42.110:/home/dan/dev/AVR/test_write_data
rm *.tar
clear
