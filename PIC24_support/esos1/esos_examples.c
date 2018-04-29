// skeleton example of 2 tasks - test1 task sends mail to cmd_param task

ESOS_USER_TASK(cmd_param);
ESOS_USER_TASK(test1);
...
ESOS_USER_TASK(cmd_param)
{
    static uint16_t data;
    
    ESOS_TASK_BEGIN();

    while(TRUE)
    {
        ESOS_TASK_WAIT_FOR_MAIL();
        while(ESOS_TASK_IVE_GOT_MAIL())
        {
			// this can also be 8 bytes
            data = __esos_CB_ReadUINT16(__pstSelf->pst_Mailbox->pst_CBuffer);
			// do something with data
        }
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(test1)
{
    static ESOS_TASK_HANDLE cmd_param_task;
    static uint16_t _cmd_param = 0;
    
    ESOS_TASK_BEGIN();
    cmd_param_task = esos_GetTaskHandle(cmd_param);

    while(TRUE)
    {
		// set _cmd_param to something
		__esos_CB_WriteUINT16(cmd_param_task->pst_Mailbox->pst_CBuffer,_cmd_param);
		ESOS_TASK_WAIT_TICKS(2000);
	}
    ESOS_TASK_END();
}	
void user_init(void) 
{
	...
    esos_RegisterTask(test1);
    esos_RegisterTask(cmd_param);
	...
}

// skeleton example of 2 tasks using a semaphore 

ESOS_USER_TASK(task1);
ESOS_USER_TASK(task2);
ESOS_SEMAPHORE(sem1);

#define MY_DELAY 100

ESOS_USER_TASK(task1)
{
    ESOS_TASK_BEGIN();

    while(TRUE)
    {
        ESOS_SIGNAL_SEMAPHORE(sem1,1);	// send signal task2 to continue
        ESOS_TASK_WAIT_TICKS(MY_DELAY);
    }
    ESOS_TASK_END();
}

ESOS_USER_TASK(task2)
{
    ESOS_TASK_BEGIN();
    while(TRUE)
    {
        ESOS_TASK_WAIT_SEMAPHORE(sem1,1);	// wait here until task1 sends a signal to continue
        ESOS_TASK_WAIT_TICKS(MY_DELAY/2);	// even though the delay here is shorter it will effectively
											// be the same as the delay time of task1
											// if the param is 2 instead of 1 it will wait for 2 signals
											// from task1
    }
    ESOS_TASK_END();
}

void user_init(void) 
{
	...
    ESOS_INIT_SEMAPHORE(sem1,0);
    esos_RegisterTask(task1);
    esos_RegisterTask(task2);
	...
} // end user_init()

// skeleton example of waiting on chars from comm3 and sending to comm2 and comm 1

ESOS_USER_TASK(serial_port1)
{
	static uint8_t data;
    ESOS_TASK_BEGIN();
    while(TRUE)
	{
        ESOS_TASK_WAIT_ON_AVAILABLE_IN_COMM3();
        ESOS_TASK_WAIT_ON_GET_UINT83(data);
        ESOS_TASK_SIGNAL_AVAILABLE_IN_COMM3();

        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM();
        ESOS_TASK_WAIT_ON_SEND_STRING("Here's the data byte: ");
        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING(data);
        ESOS_TASK_WAIT_ON_SEND_UINT8(0x20);			// print a space character
        ESOS_TASK_WAIT_ON_SEND_STRING("/n/r");		// print a carriage return
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM();		// (what is a carriage anyway?)
		
        ESOS_TASK_WAIT_ON_AVAILABLE_OUT_COMM2();
        ESOS_TASK_WAIT_ON_SEND_UINT8_AS_HEX_STRING2(data);
        ESOS_TASK_SIGNAL_AVAILABLE_OUT_COMM2();
	}
	ESOS_END_TASK();
}

// timers
// using the ESOS_USER_TIMER the fastest possible f is 500Hz (2ms)
// and using the ESOS_WAIT_TICKS(1) = 250Hz (4ms)

ESOS_USER_TASK(test_RE7)
{
    static  uint8_t data = 2;
    CONFIG_RE7_AS_DIG_OUTPUT();
    ESOS_TASK_BEGIN();
    while(1)
    {
	    ESOS_TASK_WAIT_TICKS(1);
	    _LATE7 = ~_LATE7;
    } // endof while()
    ESOS_TASK_END();
}

ESOS_USER_TIMER(testTimer)
{
    _LATG15 = ~_LATG15;
}

void user_init(void)
{
    CONFIG_RG15_AS_DIG_OUTPUT();
	esos_RegisterTimer( testTimer, 1);
	esos_RegisterTask(test_RE7);
}


