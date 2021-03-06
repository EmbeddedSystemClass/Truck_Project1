echo "running try_sched.sh (v1.14)" >> status.txt
date >> status.txt

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
  ./sched2 test 1
  OUT=$?

if [ $OUT -eq 10 ]
  then
   echo "download ok" >> status.txt
   mv sched2 sched
   chmod +x sched
   /sbin/reboot
else 
 echo "bad sched download" >> status.txt
  rm sched2
  /sbin/reboot
fi
fi

if [ $OUT -eq 5 ]
 then
  echo "downloading new param.conf" >> status.txt
  ./server >> status.txt
  ./show_params 1 >> status.txt
  OUT=$?

if [ $OUT -eq 11 ]
  then
   echo "param.conf download ok" >> status.txt
   mv sched2 param.conf
   /sbin/reboot
else 
 echo "bad param.conf download" >> status.txt
  rm sched2
  /sbin/reboot
fi
fi

if [ $OUT -eq 6 ]
 then
  if [ -e sched2 ]
   then
   echo "exit to shell and rerun sched" >> status.txt
   mv sched2 sched
   ./sched odata.dat
   ./try_sched.sh
   else 
   echo "sched2 not found" >> status.txt
   /sbin/reboot
  fi
fi 

if [ $OUT -eq 141 ]
 then
  echo "rebooting from 141" >> status.txt
fi

if [ $OUT -eq 139 ]
 then
  echo "rebooting from 139" >> status.txt
fi
