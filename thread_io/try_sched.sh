if [ -e sched2 ]
then
 mv sched2 sched
 chmod +x sched
 echo "file found" > found.txt
else
 echo "file not found" > found.txt
fi
./sched
OUT=$?
if [ $OUT -eq 0 ]
 then
 echo "exiting" > status.txt
 else if [ $OUT -eq 1 ]
 then
 echo "rebooting" > status.txt
 /sbin/reboot
 else if [ $OUT -eq 2 ]
 then
 echo "shutdown" > status.txt
 /sbin/shutdown -h now
 fi
fi
fi
