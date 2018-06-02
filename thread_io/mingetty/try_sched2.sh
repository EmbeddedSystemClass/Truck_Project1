echo "v1.05"
../mountcf.sh
if [ -e /mnt/cf/bootup/sched ]
then
 echo "new sched found"
 mv /mnt/cf/bootup/sched .
 chmod +x sched
fi
if [ -e /mnt/cf/bootup/try_sched.sh ]
then
 echo "new try_sched.sh found"
 mv /mnt/cf/bootup/try_sched.sh .
 chmod +x try_sched.sh
fi

if [ -e sched2 ]
then
 mv sched2 sched
 chmod +x sched
# echo "file found" > found1.txt
 echo "file found"
else
# echo "file not found" > found2.txt
 echo "file not found"
fi
./sched idata.dat odata.dat
OUT=$?
echo "OUT:"
echo $OUT
if [ $OUT -eq 0 ]
 then
 echo "exiting" > status.txt
 echo "exiting from script"
 else if [ $OUT -eq 1 ]
 then
 echo "rebooting" > status.txt
 echo "rebooting from script"
# /sbin/reboot
 else if [ $OUT -eq 2 ]
 then
 echo "shutdown" > status.txt
 echo "shutdown from script"
# /sbin/shutdown -r now
 fi
fi
fi