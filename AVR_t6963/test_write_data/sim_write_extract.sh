clear
tar xvf sim_write.tar
make -f make_pic clean &> out.txt
mv *.c *.h ..
mv ../sim_write.c .
make -f make_pic &> out.txt
rm *.o
mv *.tar temp
sudo ./sim_write
