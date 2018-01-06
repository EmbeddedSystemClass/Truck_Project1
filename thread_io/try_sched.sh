if [ -e sched2 ]
then
 mv sched2 sched
 chmod +x sched
 echo "file found" > found.txt
else
 echo "file not found" > found.txt
fi
if ./sched
 then
 echo "exiting" > status.txt
 exit
else 
 echo "rebooting" > status.txt
 /sbin/reboot
fi  
