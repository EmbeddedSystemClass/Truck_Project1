Truck_Project1

I created this repo because the separate repos:

Truck_App1
PIC24_support
thread_io
AVR_t6963

were all related to each other. I tried using some of the linking features of github and found them utterly useless.
Then I tried using the tag feature of gitk and found they don't carry over when pusing up to github. So I just
decided to make a new project that has each of the former repos above as subdirectories within the Truck_Project1
repo.

test_write_data.c is used to test sending sample data to the AVR. It can also be used to test the sending/receiving<br />
of data to another linux box (with serial cables connected). One one box run 'test_data' with no params to read,<br />
and on the other, run 'test_data <iters> <starting rpm value> <starting value for all others><br />
<li>pic writes START_AVR in IDLE then goes to GET_REQ</li>
<li>avr reads START_AVR then goes to REQ_DATA</li>
<li>avr writes PIC24_GET_DATA then goes to WAIT_DATA_READY</li>
<li>pic reads PIC24_GET_DATA then goes to GET_DATA</li>
<li>pic writes PIC24_DATA_READY then goes to P24_WAIT_NEW_DATA</li>
<li>avr reads PIC24_DATA_READY then goes to AVR_WAIT_NEW_DATA</li>
<li>avr writes AVR_HAS_NEW_DATA then goes to SEND_NEW_DATA</li>
<li>avr goes to AVR_IDLE</li>
<li>pic reads AVR_HAS_NEW_DATA then goes to P24_STORE_NEW_DATA</li>
<li>pic goes to IDLE</li>
