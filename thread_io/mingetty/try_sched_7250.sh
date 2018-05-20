echo "running try_sched.sh (v1.08)" >> status.txt

if [ -e sched2 ]
then
 mv sched2 sched
 chmod +x sched
 echo "new sched found" >> found1.txt
else
 echo "using current sched" >> found2.txt
fi
./sched idata.dat odata.dat
OUT=$?
echo "OUT:"
echo $OUT
if [ $OUT -eq 0 ]
 then
 date >> status.txt
 echo "exiting from script" >> status.txt
 else if [ $OUT -eq 1 ]
 then
 date >> status.txt
 echo "rebooting from script" >> status.txt
 /sbin/reboot
 else if [ $OUT -eq 2 ]
 then
 date >> status.txt
 echo "shutdown from script" >> status.txt
 /sbin/shutdown -h now
 fi
fi
fi
