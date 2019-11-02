<h1>Truck_Project1</h1>
<br />
Truck_Project1 is an automotive SCADA system which uses a Xilinx Spartan-3E to <br />
monitor real-time data, a STM32F100RB to handle a keypad and route message<br />
traffic, and a TS-7200/7800 to do I/O (turning on and off lights and reading switches).<br />
The STM32F100RB (STM32) will route message traffic to and from the FPGA, the <br />
TS-7200 and handle the keypad input.<br />
<img src="Images/SystemDiagram.JPG">
<br />
A Windows 10 TCP client runs on a laptop and the VGA port is connected to an external 8"<br />
monitor in the dash as the UI<br />
<img src="Images/ClientUI.JPG">
Because the fact that the STM32 has no ethernet connection, and only communicates with<br />
the TS-7200 via a serial port, the message passing gets really complicated.<br />
Add to that, the fact that I want to be able to send messages using the keypad as well as<br />
the Windows client to the TS-7200.<br />
<h2>Using the keypad</h2>
The keypad buttons, for now, do the basic functions I need:<br />
<li> '1' - start engine</li>
<li> '2' - stop engine</li>
<li> '3' - brights</li>
<li> '4' - cooling fan</li>
<li> '5' - heater blower</li>
<li> '6' - running lights</li>
<li> '7' - headlamps</li>
<li> '8' - wipers</li>
<li> '9' - not used</li>
<li> '0' - not used</li>
Note: when you press '1' to start the engine, it only powers the relays for the ignition,<br />
fuel pump, and completes a circuit to a button on the dash to power the starter solinoid.<br />
The, the code in the TS-7200 only gives you 10 seconds to start the engine. After that,<br />
the starter solinoid is disabled.<br />
The 'stop engine' button (2) will turn off the ignition and fuel pump<br />
The keys: A->D navigate the bottom 10 buttons on the windows client starting with 'Start Engine'<br />
going down to 'Settings Three', then up to 'Play List' and then down to 'Settings Four'.<br />
The system will operate without the laptop, but you can only use the numbered buttons on<br />
the keypad. The messages generated by the lettered buttons are routed to the Windows client<br />
The buttons labeled 'Test Ports', 'Settings One/Two/Three/Four' call up child dialogs that<br />
send messages to the TS-7200. The text on the button and the message it sends can be edited<br />
by changing xml files.<br />
<h2>The STM32 processor</h2>
The STM32F100RB is in the subdirectory 'STM32' and was generated using STM32CubeMX and compiled<br />
with TrueStudio. The project uses FreeRTOS as the operating system. Currently, all but 3 pins<br />
are used so the next step is to upgrade to a better STM32 chip.<br />
<img src="Images/STM32pinout.JPG">
What's shown here is the pinout that STM32CubeMX reports. I added some user code in the gpio.c<br />
file that added the pins for the 2 DS1620's. There are sections in the generated code that<br />
are reserved for user editing. Anything outside these sections gets removed if you pull the<br />
project back into CubeMX. I had to do it this was because the data pin on the DS1620 is<br />
changed from input to output. One of the routines generated is a callback for a 1 second<br />
timer which reads the temp settings to turn the cooling fan on or off according to what the<br />
DS1620 for engine temp reads. (The DS1620 is mounted on the thermostat housing). The other<br />
DS1620 is in the cab and reads the indoor temp. A section in the timer callback turns on/off<br />
the blower for the heater depending on the indoor temp.<br />
<h2>The Xilinx processor</h2>
I use a Spartan-3E Xilinx FPGA board to monitor real-time data like engine RPM,MPH. It sends<br />
data to the STM32 over a parallel port. The STM32 sends data to the FPGA board over a<br />
RS-232 port and uses handshaking signal. Two sensors are connected to the FPGA board which<br />
are converted to RPM/MPH readings. One sensor comes from a hall-effect switch on the driveshaft<br />
and another comes from a light sensor on the crankshaft. The FPGA also gets messages from the<br />
STM32 to generate DTMF tones whenever the keys on the keypad are pushed. The data from the<br />
RPM/MPH is sent over a parallel port to the STM32 which sends it on to the client and also<br />
sent to an XMEGA processor over a serial line which drives two LCD displays mounted on the dash.<br />
<h2 The Home Server</h2>
<img src="Images/ClientUI2.JPG">
I'm currently working on another Windows C# app which works just like the client, only it's a TCP<br />
server which runs on the desktop in the garage and only works when the truck is parked just outside.<br />
There are 2 network connections on the client. One is threw an ethernet cross-over cable plugged<br />
directly into the TS-7200 iobox and the other is a WiFi connection. When I start the client app<br />
on the laptop, I have to connect to the TS-7200 using the top-left button called 'Connect'.<br />
Then I can close the lid on the laptop, put it under the seat and use the keypad to navigate<br />
the 10 buttons on the bottom. The button called 'Call Home' will connect to the Home Server<br />
and then I can go back in the garage and while the engine is warming up, set config parameters<br />
and control the lights, blower and shut off the engine for what it's worth.<br />

