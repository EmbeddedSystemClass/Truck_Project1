<h1>Truck_Project1_notes.txt 8/1/2017</h1>
<br />
[demo video](https://www.youtube.com/watch?v=zaYD_nlhRvk)<br />
<br />
<br />
8/7/17<br />
test AVR to FPGA SPI - AVR just echos back what's recv'd in SPI to FPGA which prints results to serial port<br />
connection to SPI on AVR will have to be removeable to program to AVR - if this works then start wiring<br />
the master board together<br />
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
<br />
FPGA to AVR works up to ~30K doing just an echo<br />
<br />
8/18/17<br />
idea for new branch (PIC24_menus)<br />
<br />
(in the old way)<br />
if fptr field in menu_structs is 0 then goto the menu in the menu field, otherwise execute the function<br />
in the fptr array of function pointers.<br />
<br />
MENU_FUNC_STRUCT;<br />
<br />
(the old way)<br />
<br />
	UCHAR enabled;		// if shown on the menu<br />
	UCHAR fptr;			// which function to call (menu_types)<br />
	UCHAR menu;			// which menu to goto if _menu_change is the fptr<br />
	UCHAR label;		// which label to display in legend (labels)<br />
	UCHAR index;		// if > 0 then this is index into sample_data<br />
<br />
(the new way)<br />
<br />
	UCHAR fptr;								// which function to call (menu_types)<br />
	int menus[6];							// which menu to goto if _menu_change is the fptr<br />
	UCHAR index;							// if > 0 then this is index into sample_data<br />
<br />
<br />
types of function ptrs can be:<br />
<br />
- goto next menu (use the label param as index to next menu)<br />
- execute a choice in the checkbox-like list<br />
- execute a function like: init_checkboxes or start_numentry<br />
- do a key entry<br />
- do a 'non-function' which only means something to the PIC24<br />
<br />
If the AVR is rebooted then it will send a cmd to the PIC24 (upon startup) saying to re-upload the configuration files.<br />
If the PIC24 is rebooted, it will (optionally) send a cmd saying that it needs to re-upload the config files.<br />
(or push the data using PUSH_DATA cmd)<br />
In the AVR, it could be a if statement in the main loop that filters out that cmd.<br />
<br />
AVR_menu.c	- runs in AVR<br />
PIC_menu.c	- in PIC24<br />
<br />
sim_write.c compiles PIC_menu.c and simulates the PIC24<br />
sim_read.c compiles AVR_menu.c and simulates the AVR<br />
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
bug list:<br />
- find out why columns are only 30 (should be 40) font size?<br />
<br />
new idea: make all possible labels exist in eeprom and use an array of indexes to find any given<br />
label which is terminated by a '0' and just send over the indexes into the labels when displaying<br />
either the menus, the list of checkbox choices or the exec choices. Then have an array of menus_labels,<br />
checkbox_labels and execchoice_labels that are bigger than what's currently displayed and implement<br />
a type of garbage-collecting algorithm which saves the latest labels used so we don't have to access<br />
the eeprom every time. (NOT!)<br />
<br />
Also, make the exec_choices execute the do_numentry function so all exec_choices are pointing to<br /> 
a list of numbers to edit. <br />
<br />
The AVR_menu.c could have a function that accesses the eeprom with an #ifdef which when run in the <br />
simulator, will access an array which represents what's in the eeprom.<br />
<br />
There can be a 'push_data' function called from the laptop (via thread_io) which burns new labels<br />
into the eeprom. Longer labels can be in a different array with longer length so when burning new<br />
labels, we don't have to reprogram the entire eeprom, just do an eeprom update. <br />
<br />
Or just, for now, have label arrays in eeprom with fixed lengths until we start running out of <br />
space in the eeprom so we can just do updates on that array index. Also, put the menu_structs in <br />
the eeprom too, but always have a copy in memory.<br />
<br />
0x300 can be the start of where the location variables are:<br />
- start of short labels (10 bytes per record)<br />
- start of long labels (30 bytes per record)<br />
- no. of short labels<br />
- no. of long labels<br />
<br />
There is no reason to store the menu_structs in eeprom. The menu_structs is stored in the PIC.<br />
<br />
There is a function called get_label in the AVR_menu.c which corrisponds to the menu_label array<br />
in the PIC, but the function is separated by the MAIN_C define. The labels can just start at<br />
the beginning of eeprom and the long labels can start at some predefined offset so that when<br />
reprogramming the labels we can just use an update. A function called get_label_offset reads<br />
threw all the labels and fills an array of ints called label_offsets in the AVR. The label_offsets<br />
function will have to be run each time from the init_list function in the AVR. <br />
<br />
If the AVR gets labels from the eeprom using the label_offsets array, then the simulation should<br />
do the same thing, only using an array of 1023 UCHAR's to represent the eeprom. So in all the<br />
update functions in sim_write.c just use defines that make the chip version read and write eeprom<br />
while the simulation reads/writes the 1023 array. So both the PIC and AVR simulations will have to<br />
run the update functions to load the eeprom_sim array, but the chip will not run the update functions<br />
except when reprogramming the eeprom. <br />
<br />
Another nice feature would be 'bread-crumbs' e.g. MAIN->MENU1A->MENU1B-> ...<br />
In that case '*' could always be 'back-to-MAIN' and 'B' could be <back><br />
<br />
Tried to store the MENU_FUNC_STRUCT's in the AVR but ran out of memory unless I reduced the <br />
AUX_STRING_LEN to 50. Was trying to read it into the menu_structs array to write it to eeprom.<br />
The idea being that the PIC could read it from the eeprom on init and then optionally load a<br />
new one.<br />
<br />
The eeprom.bin file can be edited on the laptop using the sim programs and sent via the ncurses <br />
monitor program to the TS-7200 and then relayed to the PIC24 where it can sent with an <br />
BURN_EEPROM cmd to the AVR. <br />
<br />
The PIC can load the menu_structs array with a default menu using the update_menu_structs()<br />
while the laptop and/or the TS-7200 can send over a new one. The write simulation creates a file<br />
called eeprom.bin which is the binary copy of the eeprom. The technician can make a new eeprom,<br />
send it over to the PIC with a command to burn it to the eeprom.<br />
<br />
The AVR has the labels in eeprom, but the prompts for the checkboxes must be sent over everytime.<br />
There is enough memory to have a buffer that stores several sets of the most recent checkbox<br />
prompts. Could make a sort of MRU (most recently used) algorithm.<br />
<br />
sim_write must burn the eeprom first, then (optionally) read the eeprom, then load the <br />
menu_structs.<br />
<br />
When sim_write is connected to AVR, it only needs to read the eeprom (if already burnt),<br /> 
then load the menu_structs.<br />
<br />
"make set_eeprom_save_fuse" to set fuses so eeprom is saved when reprogramming<br />
"make clear_eeprom_save_fuse" to set fuses so eeprom gets erased everytime<br />
<br />
9/16/17 - using comm1 on PIC to go to monitor and comm2 goes to AVR. Testing the loop in<br />
the comm2_task (for menuing) and in comm1_task (burning eeprom). Either one can be run<br />
from the keypad or the keyboard. poll_comm2 polls the keyboard (comm1) and sends mail to<br />
the comm1/2_task. <br />
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
9/21/17<br />
Date:   Thu Sep 21 11:22:13 2017 -0600<br />
    pushed checkbox structs at starup just after sending eeprom data<br />
Date:   Thu Sep 21 08:12:01 2017 -0600<br />
    messed with PIC->read_sim - had all kinds of problems, <br />
    probably needs decoupling caps - went back to sim_write->sim_read <br />
    and commented out xfer of size,start_add and aux_string<br />
Date:   Tue Sep 19 21:46:19 2017 -0600<br />
    tested better way to parse sync frame - test_rw1a/b.c<br />
<br />
currently the ESOS stuff is doing weird shit - prolly need caps<br />
went back to sim_write->sim_read and ended up pushing the entire eeprom <br />
data along with TOTAL_NUM_CHECKBOXES of the checkboxes arrays.<br />
<br />
