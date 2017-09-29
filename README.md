<h1>Truck_Project1</h1>
<br />
This project has a subdirectory of each of the 4 processors used in the system<br />
<li>PIC24_support - A PIC24 processor that uses ESOS and acts as the master</li>
<li>Truck_App1 - A XILINX Spartan3E FPGA that reads real time information</li>
<li>AVR_t6963 - A ProMini 328p AVR that drives an LCD display</li>
<li>thread_io - A TS-7200 with 2 20 port I/O cards for handling IO</li>
<br />
Basically, the  PIC24FJ128GB110 will route message traffic to and from the FPGA, the TS-7200<br />
and the LCD screen. The AVR chip is the driver for the LCD screen and just gets commands from<br />
the PIC24FJ128GB110 over the serial port. The path to the FPGA is the SPI and the path to the<br />
AVR and the TS-7200 are serial ports. The keypad input manages the menuing of the LCD screen<br />
and can display optionally real time info like engine speed, engine temp, oil pressure, etc.<br />
There will also be a task to read the ADC's which measure temp sensors and the oil pressure.<br />
The FPGA has 2 serial ports (transmit only) that send rpm & mph data to a couple of off-the-<br />
shelf 4 digit LED's that will be on the dashboard. The FPGA also sends the rpm & mph data to<br />
the PIC24FJ128GB110 via the SPI (as master).<br />
Other telemetry data can be added later like distributor timing to use for spark advance.<br />
The TS-7200 has a removable (not hot-swapable) compact flash disk so the idea is to have a <br />
'black box' FDR that records the last so many hours or minutes of the telemetry data to a<br />
file that can be downloaded later.<br />
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
<br />
FPGA to AVR works up to ~30K doing just an echo<br />
<br />
<br />
<h2>PIC24</h2>
<br />
Truck_App2.c is the main module that runs on the PIC24FJ128GB110 chip.<br />
It uses the ESOS library. Each ESOS_USER_TASK will run independently.<br />
Each task must be registered in user_init.<br />
<br />
<br />
// skeleton example of 2 tasks - test1 task sends mail to cmd_param task<br />
<br />
ESOS_USER_TASK(cmd_param);<br />
ESOS_USER_TASK(test1);<br />
...<br />
ESOS_USER_TASK(cmd_param)<br />
{<br />
    static uint16_t data;<br />
    <br />
    ESOS_TASK_BEGIN();<br />
<br />
    while(TRUE)<br />
    {<br />
        ESOS_TASK_WAIT_FOR_MAIL();<br />
        while(ESOS_TASK_IVE_GOT_MAIL())<br />
        {<br />
			// this can also be 8 bytes<br />
            data = __esos_CB_ReadUINT16(__pstSelf->pst_Mailbox->pst_CBuffer);<br />
			// do something with data<br />
        }<br />
    }<br />
    ESOS_TASK_END();<br />
}<br />
<br />
ESOS_USER_TASK(test1)<br />
{<br />
    static ESOS_TASK_HANDLE cmd_param_task;<br />
    static uint16_t _cmd_param = 0;<br />
    <br />
    ESOS_TASK_BEGIN();<br />
    cmd_param_task = esos_GetTaskHandle(cmd_param);<br />
<br />
    while(TRUE)<br />
    {<br />
		// set _cmd_param to something<br />
		__esos_CB_WriteUINT16(cmd_param_task->pst_Mailbox->pst_CBuffer,_cmd_param);<br />
		ESOS_TASK_WAIT_TICKS(2000);<br />
	}<br />
    ESOS_TASK_END();<br />
}	<br />
void user_init(void) <br />
{<br />
	...<br />
    esos_RegisterTask(test1);<br />
    esos_RegisterTask(cmd_param);<br />
	...<br />
}<br />
<br />
// skeleton example of 2 tasks using a semaphore <br />
<br />
ESOS_USER_TASK(task1);<br />
ESOS_USER_TASK(task2);<br />
ESOS_SEMAPHORE(sem1);<br />
<br />
#define MY_DELAY 100<br />
<br />
ESOS_USER_TASK(task1)<br />
{<br />
    ESOS_TASK_BEGIN();<br />
<br />
    while(TRUE)<br />
    {<br />
        ESOS_SIGNAL_SEMAPHORE(sem1,1);	// send signal task2 to continue<br />
        ESOS_TASK_WAIT_TICKS(MY_DELAY);<br />
    }<br />
    ESOS_TASK_END();<br />
}<br />
<br />
ESOS_USER_TASK(task2)<br />
{<br />
    ESOS_TASK_BEGIN();<br />
    while(TRUE)<br />
    {<br />
        ESOS_TASK_WAIT_SEMAPHORE(sem1,1);	// wait here until task1 sends a signal to continue<br />
        ESOS_TASK_WAIT_TICKS(MY_DELAY/2);	// even though the delay here is shorter it will effectively<br />
											// be the same as the delay time of task1<br />
											// if the param is 2 instead of 1 it will wait for 2 signals<br />
											// from task1<br />
    }<br />
    ESOS_TASK_END();<br />
}<br />
<br />
void user_init(void) <br />
{<br />
	...<br />
    ESOS_INIT_SEMAPHORE(sem1,0);<br />
    esos_RegisterTask(task1);<br />
    esos_RegisterTask(task2);<br />
	...<br />
} // end user_init()<br />
<br />
// skeleton example of waiting on chars from comm3 and sending to comm2 and comm 1<br />
<br />
ESOS_USER_TASK(serial_port1)<br />
{<br />
	static uint8_t data;<br />
    ESOS_TASK_BEGIN();<br />
    while(TRUE)<br />
	{<br />
        ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM3();<br />
        ESOS_TASK_WAIT_ON_GET_UINT83(data);<br />
        ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM3();<br />
<br />
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();<br />
        ESOS_TASK_WAIT_ON_SEND_STRING("Here's the data byte: ");<br />
        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data);<br />
        ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);			// print a space character<br />
        ESOS_TASK_WAIT_ON_SEND_STRING("/n/r");		// print a carriage return<br />
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();		// (what is a carriage anyway?)<br />
		<br />
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();<br />
        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING2(data);<br />
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();<br />
	}<br />
	ESOS_END_TASK();<br />
}<br />
<br />
// timers<br />
// using the ESOS_USER_TIMER the fastest possible f is 500Hz (2ms)<br />
// and using the ESOS_WAIT_TICKS(1) = 250Hz (4ms)<br />
<br />
ESOS_USER_TASK(test_RE7)<br />
{<br />
    static  uint8_t data = 2;<br />
    CONFIG_RE7_AS_DIG_OUTPUT();<br />
    ESOS_TASK_BEGIN();<br />
    while(1)<br />
    {<br />
	    ESOS_TASK_WAIT_TICKS(1);<br />
	    _LATE7 = ~_LATE7;<br />
    } // endof while()<br />
    ESOS_TASK_END();<br />
}<br />
<br />
ESOS_USER_TIMER(testTimer)<br />
{<br />
    _LATG15 = ~_LATG15;<br />
}<br />
<br />
void user_init(void)<br />
{<br />
    CONFIG_RG15_AS_DIG_OUTPUT();<br />
	esos_RegisterTimer( testTimer, 1);<br />
	esos_RegisterTask(test_RE7);<br />
}<br />
<br />
Special character codes sent by the PIC24:<br />
<br />
RT_RPM<br />
RT_ENGT<br />
RT_TRIP<br />
RT_TIME<br />
RT_AIRT<br />
RT_MPH<br />
RT_OILP<br />
RT_MAP<br />
RT_OILT<br />
RT_O2<br />
RT_AUX1<br />
RT_AUX2<br />
<br />
<br />
<h2>AVR_t6963</h2>
<br />
The AVR is just a driver for the LCD display. The serial port of the AVR communicates with a PIC24.<br />
The PIC24 sends a stream of data specified by an array of structs rt_params.<br />
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
When the PIC24 sends a byte in the range of 0xE0 -> 0xEF that will be interpreted as<br />
a keypress. The dialog will filter this out using a switch statement. The default will<br />
just return whatever was passed in. (see get_key() in menus.c)<br />
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
<h3>PIC:</h3>
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
<h3>AVR:</h3>
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
<br />
currently the ESOS stuff is doing weird shit - prolly need caps<br />
went back to sim_write->sim_read and ended up pushing the entire eeprom <br />
data along with TOTAL_NUM_CHECKBOXES of the checkboxes arrays.<br />
<br />
test_write_data:<br />
<br />
The subdirectory called test_write_data has 2 separate makefiles to compile the sim_read and<br />
sim_write:<br />
<br />
make -f make_avr - compiles the sim_read<br />
make -f make_pic - compiles the sim_write<br />
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
Currently we must, just after the eeprom image is sent over, hit the 'h' key on the sim_write <br />
which does a LOAD_RAM command which sends over all the checkbox prompts (cblabels array),<br />
current state of the checkboxes (checked or not), the indexes for the checkboxes, the<br />
rt_params array of structs, and the sample_numbers array for the edit boxes.<br />
<br />
keypad layout - since the six reassignable keys are on the outside (bottom and left) it makes<br />
sense to use these as special function keys.<br />
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
_do_chkbox,	ckup,   ckdown, cktoggle, ckenter, ckesc, cclear, MENU1C	(MENU1C)<br />
_do_chkbox,	ckup,   ckdown, cktoggle, ckenter, ckesc, cclear, MENU1D	(MENU1D)<br />
<br />
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
from the linux laptop to the RJ-45 jack).<br />
<br />
The dashboard will have programming connectors for:<br />
1) the PIC24 - 6 pin din<br />
2) the AVR - USB connector to the buspirate board *<br />
3) 14 pin ICD connector to program the FPGA<br />
* or if we want to use the SPI then the MISO, MOSI, etc could have a disconnect<br />
<br />
The TS-7200 has a reset button on board and can be re-wired to bring it out to the<br />
dashboard. The PIC24 and the AVR can have separate switches to turn each board on<br />
and off. <br />
<br />
So the 2 DB9 connectors, the 3 programming jacks, the RJ-45 jack, the reset buttons<br />
and the on/off switches can be in a panel with a cover so it can be opened only for<br />
upgrading, testing and debugging.<br />
<br />
FPGA connections:<br />
<br />
1) 2 inputs for the mph & rpm and alternately another rpm for the optional<br />
		missing tooth wheel.<br />
2) a tone-generator output which goes to a speaker.<br />
3) an output which breaks the circuit to the fuel pump.<br />
4) other optional IO for future expansion.<br />
<br />
PIC24 connections:<br />
<br />
1) 8 or 16 inputs for the ADC (eng temp & oil pressure to start with)<br />
2) other IO for future expansion.<br />
<br />
The TS-7200 has connectors for the I/O - inputs from external switches or<br />
momentary contact push-buttons - outputs go either directly to<br />
lights (dash indicator lights) or to high-amp relays (headlights, starter relay, etc)<br />
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
