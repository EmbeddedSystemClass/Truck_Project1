<h1>Truck_Project1</h1>
<br />
Truck_Project1 is an automotive SCADA system which uses a Xilinx Spartan-3E to <br />
monitor real-time data, a PIC24FJ128GB110 to handle a keypad and route message<br />
traffic, a ATMEGA328P to drive a t6963 LCD screen, and a TS-7200/7800 to do<br />
I/O (turning on and off lights and reading switches).<br />
<br />
The PIC24FJ128GB110 (PIC) will route message traffic to and from the FPGA, the <br />
TS-7200 and the LCD screen.<br />
<br />
The menuing of the LCD screen and can display optional real time info like engine speed,<br />
engine temp, oil pressure etc coming directly from the FPGA.<br />
<br />
There will also be a task to read the ADC's which measure temp sensors and the oil pressure.<br />
The FPGA has 2 serial ports (transmit only) that send rpm & mph data to a couple of off-the-<br />
shelf 4 digit LED's that will be on the dashboard. The FPGA also sends the rpm & mph data to<br />
the PIC via the SPI (as master).<br />
<br />
Other telemetry data can be added later like distributor timing to use for spark advance.<br />
The TS-7200 has a removable (not hot-swapable) compact flash disk so the idea is to have a <br />
'black box' FDR that records the last so many hours or minutes of the telemetry data to a<br />
file that can be downloaded later.<br />
<br />
<h2>directories:</h2>
PIC24_support - source and libs for PIC24FJ128GB110 (uses MPLAB X)<br />
AVR_t6963 - ATMEGA328p source and support files (uses avrdude)<br />
thread_io - source for the TS-7200/7800 cards (from Technologic Systems)<br />
thread_io/ncurses - source for the ncurses program that runs on the laptop and communicates<br />
						with the TS-7200/7800.<br />
Truck_App1 - source for the FPGA (Xilinx Spartan-3E) in VHDL<br />
<br />
<h2>PIC24_support</h2>
The PIC24 uses a really nice RTOS called ESOS which is from people at Mississippi State.<br />
(see https://sites.google.com/site/pic24micro//Home/Course)<br />
within the PIC24_support directory there is /lib and /esos. lib is the src & include<br />
for using a PIC24 (or most other 16-bit controllers from Microchip*) while the esos<br />
directory is the RTOS which uses some of the /lib directory files. There is a file in<br />
the PIC24_support directory called esos_examples.c that explains how to use the ESOS system.<br />
<br />
*Microchip was bought out by AVR so you have to go to the AVR site to find the documentation.<br />
<br />
<h2>thread_io</h2>
The thread_io directory has files used to compile an embedded linux program that uses<br />
the POSIX pthread library to handle serial io and tcp/ip traffic and drive a io card that<br />
plugs into the TS-7XXX PC-104 bus. I bought the cards used on ebay. They are orginally from<br />
Tri-M Engineering. Each card has 20 inputs and 20 outputs. The outputs are small 5A relays<br />
onboard the card. The cards can be set to accessed and different memory/io addresses so up<br />
to 4 can be stacked on each other. I've tried just about every free RTOS out there and<br />
no-one seems to have the ports to work on the TS-7200 which use the ARM920T processor with<br />
the EP9301 chipset. I like the uCOS-II Jean J. Labrosse of Micrium, Weston, FL and he has<br />
the port files for the ARM processor but I wasn't inclined to take a year-long self-course<br />
on ARM assembly language, linux internals and the ARM architecture, although I did 'hack'<br />
through it for a while. So using the POSIX pthreads are a pretty good alternative and you<br />
can learn more about pthreads from 'Pthreads Programming' by Dick Buttlar from O'Reilly.<br />
<br />
<h2>Truck_App1</h2>
The Truck_App1 directory has all the files for compiling (if you want to call it that)<br />
a Spartan-3E FPGA from Xilinx. The files ending in .vhd are the VHDL programming language<br />
and I used a free copy of ISE from Xilinx. Actually, I tried to download it from the <br />
Xilinx website recently and its not exactly free since now you have to have a verifiable<br />
company and phone no. so I'm using a free copy I downloaded a couple years ago. The<br />
particular board running this is called an XC3S500E which only has the Spartan-3E chip,<br />
the clock and the pinout connectors for no less that 200 io pins. These can be purchased<br />
on ebay for about $35USD. All the other development boards have all the buttons, LEDs,<br />
DB connectors and USB ports which I didn't need. The only problem with the io connectors<br />
with these is that they are not the standard 1mm pitch that fits nicely into most<br />
perfboards which is what I'm using since I'm not into making my own printed circuit<br />
boards. If you can find a copy of ISE from Xilinx, it also comes with a simulator called<br />
ISim which lets you see how your VHDL program works and is really useful for testing<br />
and debugging the FPGA. The other alternative to Xilinx would be Altera which is probably<br />
geared more towards the hobbiest, I would think, but I'm too far down the rabbit hole<br />
now. If you want a copy of ISE, email me at hwswhacker1256@gmail.com and I'll see if<br />
I can get the file to you via ftp or something.<br />
<br />
You can learn more about VHDL and FPGA by doing a google search but here's the executive<br />
summary: <br />
<br />
With most modern micropressors and microcontrollers you write a program in C/C++ or<br />
whatever, compile it to a binary file, load it to the processor memory and the hardware<br />
on the chip executes each instruction from the binary file step by step. With the FPGA<br />
(Field Programmable Gate Array) the VHDL code lets you build the hardware or the<br />
processor core. In other words, the ISE development system actually routes the <br />
connections on the chip so you can have parallel processes running completely<br />
independent of each other in real time. This is really powerful when doing high speed<br />
data aquisition but its a steep learning curve. The ISim is really helpful and I took<br />
most of my code from the examples from the book 'FPGA Prototyping by VHDL example' by<br />
Pong. P. Chu. <br />
<br />
<h2>thread_io/ncurses</h2>
The thread_io/ncurses directory has all the source and support files for compiling a<br />
linux program that runs on any linux desktop and uses ncurses to communicate over a<br />
tcp/ip connection to the TS-7200/7800 box. The program running on the TS-7XXX box<br />
opens a couple binary files ending in .dat which are a data base where each record is<br />
a configuration for each of the 20 io ports. The dat files starting with an 'o' are<br />
for the the 20 output ports and the dat files starting with an 'i' are for the 20<br />
input ports. Basically, when there is a change on one of the inputs, it can effect<br />
any one of the 20 outputs. So that way you can assign different outputs to inputs.<br />
The ncurses program lets you change the configuration or load different dat files.<br />
This can be kind of dicey for things like the starter motor so the ioport assigned<br />
to the start switch only activates a relay that enables a button on the dash to close<br />
the circuit the the actual starter solinoid. <br />
<br />

