#! /bin/sh
 # /etc/init.d/customstartup
# copy this file to /etc/init.d
# then make a symbolic link in /etc/rc.d/rc3.d:
# ln -s ../../init.d/my_startup.sh S92my_startup.sh
# then copy mingetty (compiled for ARM) to /sbin
# then modify the end of /etc/inittab:
#ge0:2345:wait:/sbin/mingetty --autologin root ttyAM0 linux
#ge1:2345:respawn:/sbin/mingetty ttyAM0 linux
#for TS-7800:
#while in the fastboot prompt:
#delete linuxrc
#then go "ln -s linuxrc-mtdroot linuxrc"
#then go "save"
#then "exit"
#then after it comes up: "reboot"
 
 case "$1" in
   start)
#     /usr/local/bin/ts7800ctl --redledon
     ## If you are launching a daemon or other long running processes
     ## this should be started with
     # nohup /usr/local/bin/yourdaemon &
     cd /home/dan/dev/sched
     ./try_sched.sh
     ;;
   stop)
#     /usr/local/bin/ts7800ctl --redledoff
	echo "stopping sched"
     ;;
   *)
     echo "Usage: customstartup start|stop" >&2
     exit 3
     ;;
 esac
 
 exit 0
