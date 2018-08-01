echo "running try_sched.sh (v1.11)" >> status.txt
#../mountcf.sh
#if [ -e /mnt/cf/bootup/sched ]
#then
# echo "new sched found on cf" >> status.txt
# mv /mnt/cf/bootup/sched .
# chmod +x sched
#fi
#if [ -e /mnt/cf/bootup/try_sched.sh ]
#then
# echo "new try_sched.sh found on cf" >> status.txt
# mv /mnt/cf/bootup/try_sched.sh .
# chmod +x try_sched.sh
#fi

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
 fi
fi
fi
