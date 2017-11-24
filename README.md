<h1>Truck_Project1</h1>
<br />
when compiling for TS-7200:<br />
edit thread_io/Makefile so it compiles for TS-7200:<br />
<br />
#use this for TS-7200<br />
CC_FLAGS = -static -g -DMAKE_TARGET -Wstrict-prototypes -mcpu=arm920t<br />
#use this for TS-7800<br />
#CC_FLAGS = -static -g -DTS_7800 -DMAKE_TARGET -Wstrict-prototypes -mcpu=arm920t<br />
<br />
make sure target ip address is according to thread_io/client.h:<br />
<br />
#define HOST1 "192.168.42.145"	// TS-7800 with 2 IO cards used for Truck_Project<br />
#define HOST2 "192.168.42.146"	// TS-7800 with just one IO card (for now)<br />
#define Host3 "192.168.42.148"	// TS-7200 with 2 IO cards<br />
#define Host_Sim "192.168.42.110"	// simulated host on linux box<br />
<br />
and edit thread_io/ncurses/demo_menus2.c so init_client(HOSTx)<br />
calls the right HOSTx<br />
<br />
if using the make file for the simulator: thread_io/linux_make, <br />
make sure Host_Sim define in client.h is set to the ip address <br />
of the right linux box that the sched simulator is running on<br />
<br />
PIC24 UARTS:<br />
<br />
UART1 - RS232 to TS-7200<br />
UART2 - AVR<br />
UART3 - monitor<br />
<br />
UART1 uses 3v3 -> 5v -> RS-232 -> serial port to TS-7200<br />
UART2 uses 3v3 -> 5v -> serial port on AVR<br />
UART3 uses 3v3 -> 5v -> RS-232 -> serial port to monitor<br />
<br />
There is an SPI bus from the PIC24 to the FPGA: FPGA is master while PIC24 is slave.<br />
SPI from FPGA (master) to AVR (slave)<br />
(or) just one bus using (2) SS signals<br />
<br />
SPI signals from FPGA to PIC24 just need a 4k7 pull-up resistor<br />
SPI signals from FPGA to AVR need a 5<->3v3 converter (with pull-ups?)<br />
*FPGA -> PIC24 can be either RS-232 or SPI<br />
<br />
<h2>PIC24_support</h2>
<br />
Truck_App2.c is the main module that runs on the PIC24FJ128GB110 chip.<br />
It uses the ESOS library. Each ESOS_USER_TASK will run independently.<br />
Each task must be registered in user_init.<br />
<br />
Basically, the  PIC24FJ128GB110 (PIC) will route message traffic to and from the FPGA, the <br />
TS-7200 and the LCD screen. The AVR chip is the driver for the LCD screen and just gets <br />
commands from the PIC24 over the serial port. The path to the FPGA is the SPI and the <br />
path to the AVR and the TS-7200 are serial ports. The keypad input (PIC) manages<br />
the menuing of the LCD screen and can display optional real time info like engine speed,<br />
engine temp,oil pressure etc.<br />
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
<h2>AVR_t6963</h2>
<br />
The AVR is just a driver for the LCD display. The serial port of the AVR communicates with a <br />
PIC24. The PIC24 sends a stream of data specified by an array of structs rt_params.<br />
<br />
10/27/17 - the new AVR ATXMEGA128R4U has all kinds of cool features:<br />
- 30 IO pins:<br />
<br />
	PORTA (8)<br />
	PORTB (4)<br />
	PORTC (8)<br />
	PORTD (6)<br />
	PORTE (4)<br />
<br />
(2) SPI ports<br />
(5) UARTS<br />
Event Routing Network (to streamline interrupted peripherals SPI & UART?)<br />
Examples for interrupt driven UART RX<br />
<br />
LCD is driven by the AVR:<br />
<br />
PORTA -> data (0->7)<br />
PORTB.0 -> WR<br />
PORTB.1 -> CE<br />
PORTB.2 -> RST<br />
PORTB.3 -> CD<br />
PORTC.0 -> RD<br />
<br />
AVR has (2) comm ports:<br />
PORTC.2 -> TX1<br />
PORTC.3 -> RX1<br />
<br />
PORTD.2 -> TX2<br />
PORTD.3 -> RX2<br />
<br />
<br />
(main.h)<br />
typedef struct rt_params<br />
{<br />
	UCHAR row;			// row, col tells where the param will appear on screen<br />
	UCHAR col;<br />
	UCHAR shown;		// if its shown or not<br />
	UCHAR dtype;		// 0 = UCHAR; 1 = UINT; 2 = dword?<br />
	UCHAR type;			// rt_types<br />
} RT_PARAM;<br />
<br />
A 4x4 keypad is attached to the PIC24 so when a key is pressed, one of the <br />
following bytes is sent to the AVR<br />
<br />
0xE0 - '#'<br />
0xE1 - '*'<br />
0xE2 - '0'<br />
0xE3 - '1'<br />
0xE4 - '2'<br />
0xE5 - '3'<br />
0xE6 - '4'<br />
0xE7 - '5'<br />
0xE8 - '6'<br />
0xE9 - '7'<br />
0xEA - '8'<br />
0xEB - '9'<br />
0xEC - 'A'<br />
0xED - 'B'<br />
0xEE - 'C'<br />
0xEF - 'D'<br />
<br />
(Alternatively, the PIC can get the keypresses from one of the comm ports and double<br />
as the keypad)<br />
<br />
The section of code at the beginning of main.c reads from the eeprom 3 different arrays:<br />
1) labels<br />
2) rt_params<br />
3) menu_structs<br />
It also reads how many labels, rt_params and menu structs and their offsets into the eeprom.<br />
<br />
// the pic gets the key, exec's the fptr from the current menu_structs, and sends the data<br />
// the avr gets the data,<br />
// exec's the fptr based on the key sent,<br />
// the AVR should call the init before the fptr<br />
<br />
key --> PIC<br />
		fptr(key)<br />
		(send menu_index and all the menus[]<br />
		in the current menu_struct) -------------->		AVR<br />
														(receive same menu_index...)<br />
														fptr(key)<br />
<br />
the fptr's will exec all the func's in the fptr[] array of function pointers:<br />
<br />
(*fptr[NUM_FPTS])(UCHAR) = { menu_change, do_exec, do_chkbox, non_func, do_numentry, do_init };<br />
<br />
which are the same func names on both sides but they each do diff. stuff...<br />
so based on the keypress, both should exec the same func.<br />
sending the data in between each fptr being exec'd, will just set the menu_index and<br />
the rest of the menus[] in the current menu_struct. The AVR gets the curr_menu_index<br />
so when display_menus is called it does a get_mlabel for each of the curr_menus[] sent<br />
as the current menu_struct so it can display the current menu. The get_mlabel just<br />
looks up the menu strings from eeprom. The strings for the chkbox labels are stored in<br />
ram and must be sent beforehand by the LOAD_RAM cmd.<br />
<br />
so before the fptr in the AVR is called when a memu with a diff fptr<br />
(menu_change, do_chkbox, do_exec or do_numentry) it must redraw the<br />
display and if do_chkbox or do_exec, put up the current check box strings<br />
so we need to know when the AVR's fptr is called, call the init_ func for<br />
each do_ckbox, do_exec & do_numentry.<br />
<br />
PIC:<br />
<get keypress and call generic...><br />
<br />
<br />
static UCHAR generic_menu_function(UCHAR ch)<br />
{<br />
	ret_char = (*fptr[menu_structs[get_curr_menu()].fptr])(ch);<br />
<br />
	// do all the sending including the keypress<br />
<br />
	return ch:<br />
}<br />
<br />
AVR:<br />
static UCHAR generic_menu_function(UCHAR ch)<br />
{<br />
<br />
	// do all the receiving except the keypress (passed in as ch)<br />
<br />
		ret_char = (*fptr[tfptr])(ch);		// execute the function pointer from the PIC<br />
		display_menus();<br />
		if(prev_menu_index != curr_menu_index && ret_char != KP_AST)<br />
		{<br />
			switch (prev_menu_index)<br />
			{<br />
				case MAIN:<br />
				case MENU1A:<br />
				case MENU1B:<br />
				mvwprintw(win, LAST_ROW-8,1,"case 1");<br />
				break;<br />
				case MENU1C:<br />
				case MENU1D:<br />
				case MENU1E:<br />
				do_chkbox(ret_char);<br />
//				do_exec(ret_char);<br />
				mvwprintw(win, LAST_ROW-8,1,"case 2");<br />
				break;<br />
				case MENU2A:<br />
				case MENU2B:<br />
				do_exec(ret_char);<br />
//				do_chkbox(ret_char);<br />
				mvwprintw(win, LAST_ROW-8,1,"case 3");<br />
				break;<br />
				case MENU2C:<br />
				case MENU2D:<br />
				case MENU2E:<br />
				case MENU3A:<br />
				case MENU3B:<br />
				do_numentry(ret_char);<br />
				mvwprintw(win, LAST_ROW-8,1,"case 4");<br />
				break;<br />
				default:<br />
				break;<br />
			}<br />
		}<br />
		prev_menu_index = curr_menu_index;<br />
		prev_ret_char = ret_char;<br />
	}<br />
	return ret_char;<br />
}<br />
<br />
<br />
the user task poll_keypad calls get_key in PIC_menu.c with only the raw keypress and the<br />
get_key in PIC_menu.c sets an array of UCHARs to pass back out to the esos task that sends them<br />
to the AVR (9/16/17) no that's not what I ended up doing.<br />
<br />
the main.c in the AVR calls get_key in AVR_menu.c<br />
<br />
The generic_menu_func in PIC gets the keypress, passes it to the func ptr. If the menu_change fptr<br />
is called then it sets the new set of menus. Then in generic we send the fptr, ch, array of menus<br />
and aux_string then it exits and waits for another keypress.<br />
<br />
In AVR the generic reads the fptr, ch, menu array and aux_string, then exec's the function ptr,<br />
displays the new menus and returns to wait for another read. <br />
<br />
To change/add menus, add the menu type to main.h in the menu_types enum, then make sure the<br />
update_menu_labels calls in burn_eeprom() in sim_write.c match. Also make sure that<br />
NUM_MENUS in pic_main.h matches the number of calls of update_menu_structs() in sim_write.c<br />
Then add the new menu type in the case statement in set_list - this is essentially the<br />
initialization for the menu on the sim_write side (PIC). <br />
Then on the sim_read side (AVR), add the menu type in the case statement in generic_menu_function<br />
in AVR_menu.c where there is an if(prev_menu_index != menu_index).<br />
This is where the initialization occurs on the AVR when a new menu is started. <br />
However, the menu_index is the last param in the call to update_menu_structs (or the index elem<br />
in MENU_FUNC_STRUCT.<br />
<br />
If the ret_key returned from get_key() call in Truck_App.h is one of the NF_x codes from the non_func<br />
then we can do something like not send as many bytes in the loop in comm2_task or send some extra<br />
stuff to the AVR with a special cmd that's filtered by the get_key function int PIC_menu.c like<br />
read some extra data or do a reset or some init function. Very cool stuff!<br />
<br />
9/18/17 - tested the PIC24 sending data to gcc compiled test_read/write & rw2 and had<br />
all kinds of strange problems. The PIC24 reads and writes data ok using just test_read/write<br />
but test_rw1/2 do strange things. Put the sim_read on 110 and got it to work upto the <br />
point of entering numbers. Need to init the labels and rt_params in 'eeprom_sim' using<br />
the BURN_PART commands of the PIC24. Since we don't need the labels in the AVR we don't<br />
need to send over the menu_structs. Use the avr_send_data in PIC24 to init the rest of the<br />
eeprom_sim.<br />
<br />
The prompt strings for the checkboxes and execs will have to be pumped over whether<br />
using the sim or the AVR. This happens when doing an init_checkboxes or init_execchoices<br />
(The new AVR has 2k eeprom and could store more labels & prompts)<br />
<br />
The checkboxes are not mutually exclusive, the exec_choices are, so we need labels<br />
for all, but the check_boxes need only cover the number of check_boxes for do/init_checkboxes<br />
The execs only cause an event on either/both the PIC & AVR. (could have an array of fptr's<br />
for just the exec_choices)<br />
<br />
new idea for display checkbox/exec prompts:<br />
send over the size of each array of prompts just after the menu_indexes (menus[])<br />
if the choice is starting a new checkbox/exec - this will eliminate all the checkbox labels<br />
being stored in ram and doing a lookup everytime for the label.<br />
<br />
first send the high/low_byte of the total data to send,<br />
<br />
then send the number of prompts to display (byte)<br />
then send an array of indexes into the prompt string (array of bytes)<br />
<br />
then send the prompt string (array of chars)<br />
<br />
(receive using minicom on 192.168.42.115 won't receive but ~/dev/test_read.c does ???)<br />
no! 192.168.42.110 won't transmit in minicom until you exit.<br />
<br />
decided not to do this for now:<br />
{<br />
	When the Truck_App starts one of the tasks is get_sync.<br />
	It will send to comm2 a sequence of: "0x55, 0x55, 0x55, 0xAA, 0xAA, 0xAA"<br />
	assuming the AVR (or sim_read) has the syncup byte set to 0, it will<br />
	read the sequence and set the syncup byte to 1 and the done to 1.<br />
	Then it will send a sequence of "0x51, 0x52, 0xA1, 0xA2" back to the <br />
	PIC24. Then the get_sync task will wait for the correct sequence to be<br />
	sent back and wake up the comm2_task which is waiting for the<br />
	get_comm2 task to wake up which put itself to sleep when it started up.<br />
	Then get_sync will put itself to sleep. <br />
<br />
	the read() call in the loop in main of sim_read.c for the<br />
	simulator is the same as the receiveByte() call in the <br />
	loop of main.c for the AVR. So when the read() gets a char<br />
	it can call a function that checks to see if <br />
}<br />
<br />
test_write_data:<br />
<br />
The subdirectory called test_write_data has 2 separate makefiles to compile the sim_read and<br />
sim_write:<br />
<br />
make -f make_avr - compiles the sim_read<br />
make -f make_pic - compiles the sim_write<br />
<br />
4 possible setups:<br />
<br />
1) sim_write->AVR			- testing the simulator with actual AVR<br />
2) PIC->sim_read			- testing the simulator with actual PIC<br />
3) PIC->AVR					- actual PIC->AVR<br />
4) sim_write->sim_read		- using null-modem cable with 2 simulators (linux terminals)<br />
<br />
'LOAD_RAM' cmd sends over:<br />
1) cblabels[] - array of labels for checkboxes and exec prompts<br />
2) checkboxes settings<br />
3) rt_params[]<br />
2) the eeprom image (if using the simulator)<br />
<br />
in all cases update_ram() which calls update_menu_structs must be run in either <br />
the PIC or sim_write to setup the menu_structs array<br />
<br />
update_rtparams currently is sent over in LOAD_RAM but since its now burned into eeprom<br />
we can do a lookup in the AVR using read_eeprom<br />
<br />
(when testing sim_write -> AVR set the DISABLE_LCD define in make_avr and the TESTING_AVR<br />
	define in make_pic)<br />
<br />
if compiling the AVR then MAIN_C is defined	<br />
<br />
AVR_menu.c	- runs in AVR<br />
PIC_menu.c	- in PIC24<br />
<br />
domake1 script runs the make_pic makefile and if there are no errors, calls copy1.sh which<br />
tars up all the necessaray files to compile sim_write and scp copies them to the linux box<br />
used to run the sim_write (currently 192.168.42.110)<br />
<br />
domake2 script runs the make_avr makefile and if there are no errors, calls the copy2.sh which<br />
tars up all the files and copies them to the linux box to run the sim_read (currently 192.168.42.115)<br />
<br />
each linux box has a script called extract.sh which un-tars the tar file sent over and compiles<br />
everything and starts up the sim_read/write. The sim_read must be started first since it waits<br />
on a read() to start reading the eeprom image which is sent over as soon as the sim_write starts.<br />
<br />
when using the simulator, just after the eeprom image is sent over, hit the 'r' key on the<br />
sim_write which does a LOAD_RAM command which sends over all the checkbox prompts (cblabels array),<br />
current state of the checkboxes (checked or not), the indexes for the checkboxes, the<br />
rt_params array of structs, and the sample_numbers array for the edit boxes.<br />
<br />
keypad layout - since the six reassignable keys are on the outside (bottom and left) it makes<br />
sense to use these as special function keys.<br />
<br />
1 2 3 A<br />
4 5 6 B<br />
7 8 9 C<br />
* 0 # D<br />
<br />
The MENU_FUNC_STRUCT's is an array of the following struct type:<br />
<br />
typedef struct menu_func<br />
{<br />
	UCHAR fptr;								// which function to call (menu_types)<br />
	int menus[6];							// which menu to goto if _menu_change is the fptr<br />
	UCHAR index;<br />
} MENU_FUNC_STRUCT;<br />
<br />
MENU_FUNC_STRUCT menu_structs[NUM_MENUS];<br />
<br />
Here's a partial list of what the array of structs are loaded with:<br />
<br />
	fptr		menus[0]--->    							  menus[5] index<br />
<br />
_menu_change, 	MENU2C, MENU2D, MENU2E,   MENU3A, MENU3B, MENU1C, MAIN		(main)<br />
_menu_change,	MENU2B, MENU2C, MENU2D,   MENU2E, MENU3A, MENU3B, MENU1A	(MENU1A)<br />
_menu_change,	MAIN,   MENU2D, MENU1B,   MENU1D, MENU2A, MENU2B, MENU1B	(MENU1B)<br />
_do_chkbox,		ckup,   ckdown, cktoggle, ckenter, ckesc, cclear, MENU1C	(MENU1C)<br />
_do_chkbox,		ckup,   ckdown, cktoggle, ckenter, ckesc, cclear, MENU1D	(MENU1D)<br />
...<br />
<br />
When the system starts, the first menu_struct will be loaded as the current menu.<br />
the menus[] is an array of 6 ints which are indexes (defined by enum menu_types <br />
in main.h) into the list of labels in the eeprom. When the PIC gets a key to change<br />
menus it will send it over along with the menu index and also the list of menus.<br />
<br />
redefined keys for each menu choice) into an array. The 6 redefinable keys are<br />
'A'<br />
'B'<br />
'C'<br />
'D'<br />
'#'<br />
'0'<br />
<br />
 If the fptr is 0, then <br />
look up the menu specified by the menu element and just go to that menu. The function assigned <br />
to '*' in generic_menu_function() will always go back to the previous menu. <br />
<br />
This way, we have 3 possible choices:<br />
1) Use the 'factory default' set of menus in eeprom.<br />
2) Dynamically load another set of menus using a special command from the PIC24.<br />
3) #2 plus burn the new menu set into eeprom.<br />
<br />
<br />
	<br />
The array of structs will just be enough to display on the screen (or there could be more if<br />
the scrolling function can move the choices in and out)<br />
<br />
menu choices for checkboxes:<br />
A - up<br />
B - down<br />
C - set/clear<br />
D - enter<br />
<br />
when using the simulation (test_write_data) the data is sent in aux_data AUX_DATA_SIZE<br />
bytes at a time because of the fact that the serial port is set to non-blocking, there's<br />
a problem sending a variable number of bytes at a time so when it comes time to use it<br />
on the actual hardware, some things may have to be changed...<br />
<br />
keypad layout:<br />
<br />
1 2 3 A<br />
4 5 6 B<br />
7 8 9 C<br />
* 0 # D<br />
<br />
dec		hex		char<br />
35		23		#<br />
42		2a		*<br />
48		30		0<br />
49		31		1<br />
50		32		2<br />
51		33		3<br />
52		34		4<br />
53		35		5<br />
54		36		6<br />
55		37		7<br />
56		38		8<br />
57		39		9<br />
65		41		A<br />
66		42		B<br />
67		43		C<br />
68		44		D<br />
<br />
test_write_data (read) - simulates the AVR<br />
test_write_data (write) - simulates the PIC24<br />
<br />
<br />
FPGA UART2 <->  PIC24 comm2<br />
				PIC24 comm3 <-> AVR<br />
				PIC24 comm1 <-> TS-7200 comm2<br />
				<br />
The TS-7200 uses comm1 as a monitor but can be disabled and used as an extra serial port<br />
as long as the sched program can start automatically. Since I really don't need an extra<br />
serial port on the TS-7200 I can just have a DB9 connector wired out to the dashboard<br />
and possibly a switch attached to the jumper on the TS-7200 board which disabled comm1<br />
used as a monitor. The TS-7200 uses the ethernet connector as a TCP/IP server so a linux<br />
box running the client program (/dev/thread_io/ncurses) can be hooked up and and the <br />
client can log on and off at anytime.<br />
<br />
So currently there is an extra serial port available on the FPGA and one extra on the<br />
TS-7200.<br />
<br />
The dashboard will have a DB9 connector for the TS-7200 monitor and one for the FPGA<br />
plus a RJ-45 connector to the TS-7200 ethernet port. (Use a crossover ethernet cable<br />
from the linux laptop to the RJ-45 jack - or use a old D-link router as hub in case<br />
for another client to log into the TS-7200).<br />
<br />
The dashboard will have programming connectors for:<br />
1) the PIC24 - 6 pin din<br />
2) the new AVR can just use a USB connection.<br />
3) 14 pin ICD connector to program the FPGA<br />
<br />
The TS-7200 has a reset button on board and can be re-wired to bring it out to the<br />
dashboard. The PIC24 and the AVR can have separate switches to turn each board on<br />
and off. (but not the TS-7800?)<br />
<br />
So the 2 DB9 connectors, the 3 programming jacks, the RJ-45 jack, the reset buttons<br />
and the on/off switches can be in a panel with a cover so it can be opened only for<br />
upgrading, testing and debugging.<br />
<br />
FPGA connections:<br />
<br />
1) 2 inputs for the mph & rpm and alternately, another rpm for the optional<br />
		missing tooth wheel.<br />
2) a tone-generator output which goes to a speaker.<br />
3) an output which breaks the circuit to the fuel pump.<br />
4) other optional IO for future expansion.<br />
<br />
The inputs to the FPGA should have 5v clamping diodes which go to a 5v-3v3 converter.<br />
<br />
PIC24 connections:<br />
<br />
1) 8 or 16 inputs for the ADC (eng temp & oil pressure to start with)<br />
2) other IO for future expansion.<br />
<br />
The TS-7200 has connectors for the I/O (PC-104 card) - inputs from external switches or<br />
momentary contact push-buttons - outputs go either directly to lights <br />
(dash indicator lights) or to high-amp relays (headlights, starter relay, etc)<br />
<br />
TS-7200:<br />
2 UARTs are RS-232 - 1 goes to PIC24, the other is open<br />
The ehternet is for a monitor (use cross-over cable to laptop)<br />
<br />
PIC24:<br />
1 UART goes to PIC24 (5-3v3 conv to RS-232 conv)<br />
another UART goes to AVR (5-3v3 conv)<br />
SPI goes to FPGA (4k7 pull-up resistors)<br />
<br />
FPGA:<br />
1 UART goes to monitor (5-3v3 conv to RS-232 conv)<br />
<br />
"make set_eeprom_save_fuse" to set fuses so eeprom is saved when reprogramming<br />
"make clear_eeprom_save_fuse" to set fuses so eeprom gets erased everytime<br />
<br />
(10/13/17)<br />
TS-7800:<br />
Has 3 UARTS, faster CPU & better OS. Memory access is diff for DIO/LCD & PC104.<br />
No built-in SPI like the TS-7200 but very little docs on the Marvell processor. <br />
(would be nice to have interrupt routine that handles the SPI which the pins could be<br />
on the DIO. Since the TS-7800 is in a remote box we have to use serial anyway).<br />
<br />
The new AVR has (2) SPI ports and can be interrupt driven. So if I could implement the <br />
2nd SPI port that's on the PIC24 (there are interrupts avail) I could have one port as<br />
the slave coming from the FPGA and another as the master going to the AVR. The ESOS<br />
code for the SPI can be duplicated to work with the 2nd SPI port on the PIC24. <br />
<br />
The FPGA has (2) SPI's which act as masters so have one go the the AVR so it can display<br />
the realtime data on screen. The other SPI from the FPGA goes to the PIC24. (could have<br />
just the one SPI from the PIC24 act as master to the AVR for now and skip the one from<br />
the FPGA) or just use the serial port on the FPGA to send rt data to PIC24. That would<br />
free up one of the UART's on the PIC24 because it's SPI is going to the AVR. <br />
<br />
(10/25/17)<br />
added 2 branches: TS-7200 & TS-7800 but made changes in master so the Makefile in<br />
thread_io can be changed to compile for either one. (search for "-DTS-7800")<br />
also need to change the init_client(HOSTn) in demo_menus2.c and client.c in ncurses<br />
according to the defines in client.h: <br />
<br />
#define HOST1 "192.168.42.145"	// TS-7800 with 2 IO cards used for Truck_Project<br />
#define HOST2 "192.168.42.146"	// TS-7800 with just one IO card (for now)<br />
#define Host3 "192.168.42.148"	// TS-7200 with 2 IO cards<br />
<br />
Don't use the functon pointer array in the thread_io, just have dedicated threads<br />
and have the serial port one with the highest priority.<br />
<br />
Order another 2GB SD card for the TS-7800 to back up OS on.<br />
<br />
Get all the ssh's to use keygen instead of passwords<br />
<br />
Get new AVR to work with old AVR_menu and check out SPI ports.<br />
<br />
Add extra behavoir info to the I_DATA struct:<br />
<br />
typedef struct o_data<br />
{<br />
	char label[20];<br />
	UCHAR port;<br />
	UCHAR onoff;			// 1 of on; 0 if off<br />
	UCHAR type;<br />
	UINT time_delay;<br />
	UCHAR pulse_time;<br />
} O_DATA;<br />
<br />
typedef struct i_data<br />
{<br />
	char label[ILABELSIZE];<br />
	UCHAR port;<br />
	UCHAR affected_output;<br />
	UINT temp;<br />
} I_DATA;<br />
<br />
type:<br />
0) regular - on/off state doesn't change until user tells it to<br />
1) on for time delay seconds and then it goes back off<br />
2) goes on/off at a pulse_time rate until turned off again<br />
4) goes on/off at pulse_time rate for time_delay seconds and then back off<br />
<br />
10/16/17<br />
made separate make files (make_db) to compile init_db & list_db on both host and target side<br />
/thread_io (target) - sched<br />
/thread_io/ncurses (host) - client<br />
<br />
did away with curr_i_array & curr_o_array and just used the ill & oll linked lists<br />
menus:<br />
1) Host<br />
	- Open <br />
	- Save File<br />
	- Save As<br />
	- Delete<br />
	- Edit idata<br />
	- Edit odata<br />
	- Send idata<br />
	- Send odata<br />
	- Exit<br />
<br />
2) Target<br />
	- Open <br />
	- Save File<br />
	- Save As<br />
	- Delete<br />
	- List idata<br />
	- List odata<br />
	- Get idata<br />
	- Get odata<br />
<br />
3) Tool<br />
	- TCP Address<br />
	- Connect<br />
	- Disconnect<br />
	- Toggle Output<br />
	- All Off<br />
	- All On<br />
<br />
Need another database which has list of on-off times for each IO port. Have a field in<br />
the O_DATA database which points to a on-off database (or text file).<br />
In task2 when we check for a change in the inputs, check for a time-up in a list of<br />
which active time-up array and then check the time of day to see if that port should<br />
be turned on or off.<br />
<br />
Use 12vdc->5vdc converter from separate motorcycle battery to power whole system.<br />
Then have a way to keep the battery charged - either charge it from the car battery<br />
or use solar panels (or switch between both)<br />
<br />
<br />

