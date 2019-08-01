this directory has the files for testing the AVR/LCD using a TS-7200 to send the serial commands to
the AVR. Be sure and run the setARMpath.sh as long as all the arm-linux cross compiler is installed.
read/write_serial goes to the AVR while the write_serial2 goes to the COMM2 of the TS-7200 as a
monitor to a tty. the dmake3.sh script renames the file called main, compiled by the make_target2
makefile to sched because the script on the TS-7200 card looks for sched in the root directory
when it reboots and copies it to /home/dan/dev/sched using the try_sched.sh script.
the makefile called "make_target2" uses the serial_io.c/h and lcd_func.c/h in the
Truck_Project1/thread_io directory. In serial_io.c the #ifdef TEST_AVR_LCD define makes the
comm1 port 19200 instead of 115200 which is used by the sched file in thread_io.
