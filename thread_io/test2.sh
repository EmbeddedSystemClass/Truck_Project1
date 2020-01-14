unzip -o ~/thread_io.zip
mv mytypes.h ..
make clean
make &> out.txt
if grep -q error out.txt
 then
  find2 error out.txt
  exit 1
fi
#ftp 192.168.42.145
echo "ssh'ing into 145"
scp sched 192.168.0.145:/home/dan/dev/sched/sched2
rm *.o
ls -ltr sched
mv sched ~
