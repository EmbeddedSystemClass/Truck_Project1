#! /bin/sh
 # /etc/init.d/customstartup
# copy this file to /etc/rc3.d as S92my_startup.sh
# then copy mingetty (compiled for ARM) to /sbin
# then modify the end of /etc/inittab:
#ge0:2345:wait:/sbin/mingetty --autologin root ttyS0 linux
#ge1:2345:respawn:/sbin/mingetty ttyS0 linux
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
     ./sched
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
