echo "running try_sched.sh (v1.11)" >> status.txt
if [ ! -e /mnt/cf/temp.txt ]
then
 ../mountcf.sh
 echo "mounting cf"
fi
if [ -e /root/sched ]
then
 mv /root/sched sched
 chmod +x sched
 echo "new sched found in /root" >> status.txt
else if [ -e sched2 ]
then
 mv sched2 sched
 chmod +x sched
 echo "new sched found (sched2)" >> status.txt
else
 echo "using current sched" >> status.txt
fi
fi
./sched odata.dat
OUT=$?
echo "OUT:" >> status.txt
echo $OUT >> status.txt
date >> status.txt
if [ $OUT -eq 1 ]
 then
  echo "exit to shell" >> status.txt
fi
if [ $OUT -eq 2 ]
 then
  echo "rebooting from script" >> status.txt
 /sbin/reboot
fi 
if [ $OUT -eq 3 ]
 then
  echo "shutdown from script" >> status.txt
/sbin/shutdown -h now
fi
if [ $OUT -eq 4 ]
 then
  echo "downloading new sched" >> status.txt
./server >> status.txt
mv sched2 sched
chmod +x sched
/sbin/reboot
fi
if [ $OUT -eq 141 ]
 then
  echo "rebooting from 141" >> status.txt
fi
if [ $OUT -eq 139 ]
 then
  echo "rebooting from 139" >> status.txt
fi
