unzip -o ~/thread_io.zip
mv mytypes.h ..
make &> out.txt
if grep -q error out.txt
 then
 find2 error out.txt
 exit 1
fi
if grep undefined -q out.txt
 then
 find2 undefined out.txt
 exit 1
fi
ls -ltr sched
scp sched 192.168.0.145:/home/dan/dev/sched
mv sched sched2
make clean
mv sched2 sched
