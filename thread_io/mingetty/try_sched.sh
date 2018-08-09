echo "running try_sched.sh (v1.11)" >> status.txt
if [ ! -e /mnt/cf/temp.txt ]
then
 ../mountcf.sh
fi
if [ -e /root/sched ]
then
 mv /root/sched sched
 chmod +x sched
  echo "new sched found in /root" >> found.txt
fi
if [ -e sched2 ]
then
 mv sched2 sched
 chmod +x sched
 echo "new sched found" >> found.txt
else
 echo "using current sched" >> found.txt
fi
./sched idata.dat odata.dat
OUT=$?
echo "OUT:"
echo $OUT
date >> status.txt
if [ $OUT -eq 0 ]
 then
 echo "exit to shell" >> status.txt
 else if [ $OUT -eq 1 ]
 then
 echo "rebooting from script" >> status.txt
 /sbin/reboot
 else if [ $OUT -eq 2 ]
 then
 echo "shutdown from script" >> status.txt
 /sbin/shutdown -h now
 else /sbin/reboot
 fi
fi
fi
