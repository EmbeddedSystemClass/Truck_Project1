/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
#if 1
/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "string.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "dwt_stm32_delay.h"
#include "../mytypes.h"
#include "ds1620.h"	// not directly attached
// for now just use the quad sensor via the serial port (UART3)
#include "freertos_defs.h"
#include "usart.h"
#include "screen.h"
#define SEND_FPGA_DELAY() vTaskDelay(20)
//#include "adc.h"
//#include "raw_data.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define MSG_SIZE 50
typedef enum
{
	STATE_WAIT_FOR_PRESS = 1,
	STATE_WAIT_FOR_RELEASE
} ISRSTATE;

static ISRSTATE e_isrState;

const uint8_t au8_keyTable[NUM_ROWS][NUM_COLS] =
{
/*
	{KP_1, KP_2, KP_3, KP_A },
	{KP_4, KP_5, KP_6, KP_B },
	{KP_7, KP_8, KP_9, KP_C },
	{KP_AST, KP_0, KP_POUND, KP_D}
*/
	{KP_AST, KP_0, KP_POUND, KP_D},
	{KP_7, KP_8, KP_9, KP_C },
	{KP_4, KP_5, KP_6, KP_B },
	{KP_1, KP_2, KP_3, KP_A }
};

FORMAT_STR rtlabel_str[NUM_RT_LABELS];
FORMAT_STR status_label_str[NUM_STATUS_LABELS];

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint64_t pack64(UCHAR *buff);
uint32_t pack32(UCHAR *buff);
static UCHAR buff1[MSG_SIZE];
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

static int lights_on;
static int brights_on;
static int blower_on;
static int fan_on;
static int engine_on;

static int running_lights_on;
static UCHAR menu_ptr;
static KEY_MODE key_mode;
//static int key_enter_time;
static char password[PASSWORD_SIZE];
static char correct_password[PASSWORD_SIZE];
static UCHAR password_valid;
static int password_ptr;
static int password_retries;
static int dim_screen;
static int silent_key;
static UINT gl_rpm, gl_mph;
static UINT gl_engine_temp, gl_indoor_temp, gl_outdoor_temp, gl_temp4;
static UINT gl_blower_en, gl_fan_on, gl_fan_off, gl_blower1_on, gl_blower2_on, gl_blower3_on;
static int engine_run_time;
static int engine_shutdown;
static int engine_shuttingdown;
static char num_entry_num[SIZE_NUM];
//static int task7on;
static int wipers;

static void clr_scr(void)
{
	UCHAR key[2];
	key[0] = LCD_CLRSCR;
	key[1] = 0xFE;
	HAL_UART_Transmit(&huart2, key, 2, 100);
}

static void goto_scr(UCHAR row, UCHAR col)
{
	UCHAR key[4];
	key[0] = GOTO_CMD;
	key[1] = row;
	key[2] = col;
	key[3] = 0xFE;
	HAL_UART_Transmit(&huart2, key, 4, 100);
}

static void setOneRowLow(uint8_t u8_x)
{
	switch (u8_x)
	{
		case 0:
			
			HAL_GPIO_WritePin(GPIOB, row0, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, row1, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, row2, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, row3, GPIO_PIN_RESET);
			break;
		case 1:
			HAL_GPIO_WritePin(GPIOB, row0, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, row1, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, row2, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, row3, GPIO_PIN_SET);
			break;
		case 2:
			HAL_GPIO_WritePin(GPIOB, row0, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, row1, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, row2, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, row3, GPIO_PIN_SET);
			break;
		case 3:
			HAL_GPIO_WritePin(GPIOB, row0, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, row1, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, row2, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, row3, GPIO_PIN_SET);
			break;
		default:
			HAL_GPIO_WritePin(GPIOB, row0, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, row1, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, row2, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, row3, GPIO_PIN_SET);
			break;
	}
}

static void drive_row_high(void)
{
	HAL_GPIO_WritePin(GPIOB, row0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, row1, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, row2, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, row3, GPIO_PIN_SET);
}

/*
int key_is_pressed(void)
{
	GPIO_PinState state0, state1, state2, state3;

	state0 = HAL_GPIO_ReadPin(GPIOC,col0);
	state1 = HAL_GPIO_ReadPin(GPIOC,col1);
	state2 = HAL_GPIO_ReadPin(GPIOC,col2);
	state3 = HAL_GPIO_ReadPin(GPIOC,col3);
	if (state0 == GPIO_PIN_RESET || state1 == GPIO_PIN_RESET || 
					state2 == GPIO_PIN_RESET || state3 == GPIO_PIN_RESET) 
		return 1;
	
	return 0;
}
*/

int key_is_released(void)
{
	GPIO_PinState state0, state1, state2, state3;

	state0 = HAL_GPIO_ReadPin(GPIOC,col0);
	state1 = HAL_GPIO_ReadPin(GPIOC,col1);
	state2 = HAL_GPIO_ReadPin(GPIOC,col2);
	state3 = HAL_GPIO_ReadPin(GPIOC,col3);
	if (state0 == GPIO_PIN_SET && state1 == GPIO_PIN_SET && 
					state2 == GPIO_PIN_SET && state3 == GPIO_PIN_SET) 
		return 1;
	
	return 0;
}

UCHAR do_keyscan(void)
{
	uint8_t u8_row;
	UCHAR key_pressed;
	GPIO_PinState state;

	key_pressed = 0;
//	drive_row_high();

	for (u8_row = 0; u8_row < NUM_ROWS; u8_row++)
	{
		setOneRowLow(u8_row);
//			vTaskDelay(1);
		state = HAL_GPIO_ReadPin(GPIOC,col0);
		if(state == GPIO_PIN_RESET)
		{
			key_pressed = (au8_keyTable[u8_row][0]);
			return key_pressed;
		}
		state = HAL_GPIO_ReadPin(GPIOC,col1);
		if(state == GPIO_PIN_RESET)
		{
			key_pressed = (au8_keyTable[u8_row][1]);
			return key_pressed;
		}
		state = HAL_GPIO_ReadPin(GPIOC,col2);
		if(state == GPIO_PIN_RESET)
		{
			key_pressed = (au8_keyTable[u8_row][2]);
			return key_pressed;
		}
		state = HAL_GPIO_ReadPin(GPIOC,col3);
		if(state == GPIO_PIN_RESET)
		{
			key_pressed = (au8_keyTable[u8_row][3]);
			return key_pressed;
		}
	}
	return 0;
}

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
uint32_t defaultTaskBuffer[ 70 ];
osStaticThreadDef_t defaultTaskControlBlock;

osThreadId BasicCmdTaskHandle;
uint32_t myTask02Buffer[ 70 ];
osStaticThreadDef_t myTask02ControlBlock;

osThreadId KeyStateTaskHandle;
uint32_t myTask03Buffer[ 70 ];
osStaticThreadDef_t myTask03ControlBlock;

osThreadId DS1620TaskHandle;
uint32_t myTask04Buffer[ 70 ];
osStaticThreadDef_t myTask04ControlBlock;

osThreadId Task7200Handle;
uint32_t myTask05Buffer[ 70 ];
osStaticThreadDef_t myTask05ControlBlock;

osThreadId AVRTaskHandle;
uint32_t myTask06Buffer[ 70 ];
osStaticThreadDef_t myTask06ControlBlock;

osThreadId myTask07Handle;
uint32_t myTask07Buffer[ 70 ];
osStaticThreadDef_t myTask07ControlBlock;

osThreadId myTask08Handle;
uint32_t myTask08Buffer[ 70 ];
osStaticThreadDef_t myTask08ControlBlock;

osThreadId myTask09Handle;
uint32_t myTask09Buffer[ 70 ];
osStaticThreadDef_t myTask09ControlBlock;

osThreadId myTask10Handle;
uint32_t myTask10Buffer[ 70 ];
osStaticThreadDef_t myTask10ControlBlock;

osThreadId myTask11Handle;
uint32_t myTask11Buffer[ 70 ];
osStaticThreadDef_t myTask11ControlBlock;

osMessageQId keypressedQueueHandle;
uint8_t myQueue01Buffer[ 16 * sizeof( uint16_t ) ];
osStaticMessageQDef_t myQueue01ControlBlock;

osMessageQId Send7200QueueHandle;
uint8_t myQueue02Buffer[ 5 * sizeof( uint64_t ) ];
osStaticMessageQDef_t myQueue02ControlBlock;

osMessageQId SendAVRQueueHandle;
uint8_t myQueue03Buffer[ 5 * sizeof( uint64_t ) ];
osStaticMessageQDef_t myQueue03ControlBlock;

osMessageQId SendFPGAQueueHandle;
uint8_t myQueue04Buffer[ 5 * sizeof( uint64_t ) ];
osStaticMessageQDef_t myQueue04ControlBlock;

osTimerId myTimer01Handle;
osStaticTimerDef_t myTimer01ControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartBasicCmdTask(void const * argument);
void StartKeyStateTask(void const * argument);
void StartDS1620Task(void const * argument);
void StartTask7200(void const * argument);
void StartAVRTask(void const * argument);
void StartRecv7200(void const * argument);
void StartSendFPGA(void const * argument);
void StartRecvFPGA(void const * argument);
void StartTask10(void const * argument);
void StartNumEntryTask(void const * argument);
void Callback01(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
  
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}                   
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];
  
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )  
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
#endif
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
#if 1
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* definition and creation of myTimer01 */
  osTimerStaticDef(myTimer01, Callback01, &myTimer01ControlBlock);
  myTimer01Handle = osTimerCreate(osTimer(myTimer01), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */

	osMessageQStaticDef(myQueue01, 16, uint16_t, myQueue01Buffer, &myQueue01ControlBlock);
	keypressedQueueHandle = osMessageCreate(osMessageQ(myQueue01), NULL);

	/* definition and creation of myQueue02 */
	osMessageQStaticDef(myQueue02, 5, uint64_t, myQueue02Buffer, &myQueue02ControlBlock);
	Send7200QueueHandle = osMessageCreate(osMessageQ(myQueue02), NULL);

	/* definition and creation of myQueue03 */
	osMessageQStaticDef(myQueue03, 5, uint64_t, myQueue03Buffer, &myQueue03ControlBlock);
	SendAVRQueueHandle = osMessageCreate(osMessageQ(myQueue03), NULL);

	/* definition and creation of myQueue03 */
	osMessageQStaticDef(myQueue04, 5, uint64_t, myQueue04Buffer, &myQueue04ControlBlock);
	SendFPGAQueueHandle = osMessageCreate(osMessageQ(myQueue04), NULL);
  /* USER CODE END RTOS_QUEUES */
#endif

  /* Create the thread(s) */
  /* definition and creation of defaultTask */

  osThreadStaticDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 70, defaultTaskBuffer,
		&defaultTaskControlBlock);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
#if 0
  /* definition and creation of BasicCmdTask */
  //StartBasicCmdTask
  osThreadStaticDef(BasicCmdTask, StartBasicCmdTask, osPriorityIdle, 0, 70, myTask02Buffer, 
         &myTask02ControlBlock);
  BasicCmdTaskHandle = osThreadCreate(osThread(BasicCmdTask), NULL);
#endif
  /* definition and creation of KeyStateTask */
  // StartKeyStateTask
  osThreadStaticDef(KeyStateTask, StartKeyStateTask, osPriorityIdle, 0, 70, myTask03Buffer, 
		&myTask03ControlBlock);
  KeyStateTaskHandle = osThreadCreate(osThread(KeyStateTask), NULL);
#if 0
  /* definition and creation of DS1620Task */
  osThreadStaticDef(DS1620Task, StartDS1620Task, osPriorityIdle, 0, 70, myTask04Buffer, 
		&myTask04ControlBlock);
  DS1620TaskHandle = osThreadCreate(osThread(DS1620Task), NULL);

  /* definition and creation of Task7200 */
  osThreadStaticDef(Task7200, StartTask7200, osPriorityIdle, 0, 70, myTask05Buffer, &myTask05ControlBlock);
  Task7200Handle = osThreadCreate(osThread(Task7200), NULL);
#endif
  /* definition and creation of AVRTask */
  osThreadStaticDef(AVRTask, StartAVRTask, osPriorityIdle, 0, 70, myTask06Buffer, &myTask06ControlBlock);
  AVRTaskHandle = osThreadCreate(osThread(AVRTask), NULL);
#if 0
  /* definition and creation of Recv7200 */
  osThreadStaticDef(Recv7200, StartRecv7200, osPriorityIdle, 0, 70, myTask07Buffer, &myTask07ControlBlock);
  myTask07Handle = osThreadCreate(osThread(Recv7200), NULL);

  /* definition and creation of SendFPGA */
  osThreadStaticDef(SendFPGA, StartSendFPGA, osPriorityIdle, 0, 70, myTask08Buffer, &myTask08ControlBlock);
  myTask08Handle = osThreadCreate(osThread(SendFPGA), NULL);

  /* definition and creation of RecvFPGA */
  osThreadStaticDef(RecvFPGA, StartRecvFPGA, osPriorityIdle, 0, 70, myTask09Buffer, &myTask09ControlBlock);
  myTask09Handle = osThreadCreate(osThread(RecvFPGA), NULL);

  /* definition and creation of myTask10 */
  osThreadStaticDef(myTask10, StartTask10, osPriorityIdle, 0, 70, myTask10Buffer, &myTask10ControlBlock);
  myTask10Handle = osThreadCreate(osThread(myTask10), NULL);
#endif
  /* definition and creation of NumEntryTask */
  osThreadStaticDef(NumEntryTask, StartNumEntryTask, osPriorityIdle, 0, 70, myTask11Buffer, &myTask11ControlBlock);
  myTask11Handle = osThreadCreate(osThread(NumEntryTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */
}
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
void StartDefaultTask(void const * argument)
{
	int i;
	UCHAR xbyte;
	uint64_t avr_buffer[5];
	UCHAR ucbuff[8];

  /* USER CODE BEGIN StartDefaultTask */
  	menu_ptr = 0;
    password_valid = 0;
    password_ptr = 0;
	password_retries = 0;
	dim_screen = 0;
	silent_key = 0;
	gl_rpm = gl_mph = 0;
	gl_engine_temp = gl_indoor_temp = gl_outdoor_temp = gl_temp4 = 0;
	gl_blower_en = gl_fan_on = gl_fan_off = gl_blower1_on = 0;
	gl_blower2_on = gl_blower3_on = 0;
	engine_run_time = 0;
	engine_shutdown = 0;
	engine_shuttingdown = 0;

//	key_mode = PASSWORD;
	key_mode = NORMAL;
	DWT_Delay_Init();
	osTimerStart(myTimer01Handle,500);

	memset(correct_password,0,sizeof(correct_password));
	strcpy(correct_password,"2354795\0");
	memset(password,0,PASSWORD_SIZE);
//	avr_buffer[0] = PASSWORD_MODE;
	
//	vTaskSuspend(myTask07Handle);
	
	vTaskDelay(50);
	
	clr_scr();
	vTaskDelay(50);

	init_rtlabels();
//	display_rtlabels();	

	ucbuff[0] = DISPLAY_RTLABELS;
	ucbuff[1] = START_RT_VALUE_ROW;
	ucbuff[2] = START_RT_VALUE_COL;
	ucbuff[3] = ENDING_RT_VALUE_ROW;
	ucbuff[4] = RT_VALUE_COL_WIDTH;
	ucbuff[5] = NUM_RT_LABELS;

	avr_buffer[0] = pack64(ucbuff);
	xQueueSend(SendAVRQueueHandle,avr_buffer,0);
	
#if 0
//	since the UART1 receive doesn't work
//	to receive the status msg's then
//	this can be left out

	ucbuff[0] = DISPLAY_STATUSLABELS;
	avr_buffer[0] = pack64(ucbuff);
	xQueueSend(SendAVRQueueHandle,avr_buffer,0);
#endif
	xbyte = 0x21;
	for(i = 0;i < 20;i++)
	{
  		if(menu_ptr == 0)
		{
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
			vTaskDelay(50);
			menu_ptr = 1;
		}else
		{
			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
			vTaskDelay(50);
			menu_ptr = 0;
		}
	}
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);

	for(;;)
	{
		vTaskDelay(2000);
	}
	/* USER CODE END StartDefaultTask */
}
/**
* @brief get keypresses and process - mostly sending msg to 7200 task
* @param argument: Not used
* @retval None
*/
void StartBasicCmdTask(void const * argument)
{
	UCHAR key;
	UCHAR cmd;
	uint16_t recval;
	uint64_t buff[5];
	UCHAR ucbuff[8];
		
	lights_on = 0;
	brights_on = 0;
	fan_on = 0;
	blower_on = 0;
	engine_on = 0;
	wipers = 0;

  /* Infinite loop */
	for(;;)
	{
		xQueueReceive(keypressedQueueHandle, &recval, portMAX_DELAY);
		key = (UCHAR)recval;
		cmd = 0;

		switch(key)
		{
			case KP_1:
				cmd = START_SEQ;
				if(engine_on == 0)
				{
					ucbuff[0] = cmd;
					buff[0] = pack64(ucbuff);
					xQueueSend(Send7200QueueHandle, buff, 0);
					HAL_GPIO_WritePin(LD3_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
				}
			break;
			case KP_2:
				cmd = SHUTDOWN;
				ucbuff[0] = cmd;
				buff[0] = pack64(ucbuff);
				xQueueSend(Send7200QueueHandle, buff, 0);
				cmd = OFF_FAN;
				ucbuff[0] = cmd;
				buff[0] = pack64(ucbuff);
				xQueueSend(Send7200QueueHandle, buff, 0);
				engine_on = 0;
				fan_on = 0;
				HAL_GPIO_WritePin(LD3_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

			break;
			case KP_3:
				if(lights_on == 1)
				{
					if(brights_on == 0)
					{
						cmd = ON_BRIGHTS;
						ucbuff[0] = cmd;
						buff[0] = pack64(ucbuff);
						xQueueSend(Send7200QueueHandle, buff, 0);
						brights_on = 1;
					}
					else
					{
						cmd = OFF_BRIGHTS;
						ucbuff[0] = cmd;
						buff[0] = pack64(ucbuff);
						xQueueSend(Send7200QueueHandle, buff, 0);
						brights_on = 0;
					}
				}
			break;
			case KP_4:
				if(fan_on == 0)
				{
					cmd = ON_FAN;
					ucbuff[0] = cmd;
					buff[0] = pack64(ucbuff);
					xQueueSend(Send7200QueueHandle, buff, 0);
					fan_on = 1;
					HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
				}
				else
				{
					cmd = OFF_FAN;
					ucbuff[0] = cmd;
					buff[0] = pack64(ucbuff);
					xQueueSend(Send7200QueueHandle, buff, 0);
					fan_on = 0;
					HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
				}
			break;
			case KP_5:
				switch(blower_on)
				{
					case 0:	
					cmd = BLOWER_OFF;
					ucbuff[0] = cmd;
					buff[0] = pack64(ucbuff);
					xQueueSend(Send7200QueueHandle, buff, 0);
					break;
					case 1:
					cmd = BLOWER1;
					ucbuff[0] = cmd;
					buff[0] = pack64(ucbuff);
					xQueueSend(Send7200QueueHandle, buff, 0);
					break;
					case 2:
					cmd = BLOWER2;
					ucbuff[0] = cmd;
					buff[0] = pack64(ucbuff);
					xQueueSend(Send7200QueueHandle, buff, 0);
					break;
					case 3:
					cmd = BLOWER3;
					ucbuff[0] = cmd;
					buff[0] = pack64(ucbuff);
					xQueueSend(Send7200QueueHandle, buff, 0);
					break;
					default:
					blower_on = 0;
				}
				if(++blower_on > 3)
					blower_on = 0;
			break;
			case KP_6:
				if(running_lights_on == 0)
				{
					running_lights_on = 1;
					cmd = ON_RUNNING_LIGHTS;
					ucbuff[0] = cmd;
					buff[0] = pack64(ucbuff);
					xQueueSend(Send7200QueueHandle, buff, 0);
				}else
				{
					running_lights_on = 0;
					cmd = OFF_RUNNING_LIGHTS;
					ucbuff[0] = cmd;
					buff[0] = pack64(ucbuff);
					xQueueSend(Send7200QueueHandle, buff, 0);
				}
			break;
			case KP_7:
				if(lights_on == 0)
				{
					cmd = ON_LIGHTS;
					ucbuff[0] = cmd;
					buff[0] = pack64(ucbuff);
					xQueueSend(Send7200QueueHandle, buff, 0);
					lights_on = 1;
				}
				else
				{
					cmd = OFF_LIGHTS;
					lights_on = 0;
					ucbuff[0] = cmd;
					buff[0] = pack64(ucbuff);
					xQueueSend(Send7200QueueHandle, buff, 0);

					if(brights_on == 1)
					{
						cmd = OFF_BRIGHTS;
						ucbuff[0] = cmd;
						buff[0] = pack64(ucbuff);
						xQueueSend(Send7200QueueHandle, buff, 0);
						brights_on = 0;
					}
				}
			break;
			case KP_8:
				switch (wipers)
				{
					case 0:
						cmd = WIPER1;
						wipers = 1;
						break;
					case 1:
						cmd = WIPER2;
						wipers = 2;
						break;
					case 2:
						cmd = WIPER_OFF;
						wipers = 0;
						break;
					default:
						wipers = 0;
				}
				ucbuff[0] = cmd;
				buff[0] = pack64(ucbuff);
				xQueueSend(Send7200QueueHandle, buff, 0);
			break;
			case KP_9:
			break;
		}
		vTaskDelay(10);
	}
}
/**
* @brief state machine for keypad
* @param argument: Not used
* @retval None
*/
void StartKeyStateTask(void const * argument)
{
 	static UCHAR key;
	unsigned long sendval;
  
	e_isrState = STATE_WAIT_FOR_PRESS;  

	vTaskDelay(1000);
	for(;;)
	{
		switch (e_isrState)
		{
			case STATE_WAIT_FOR_PRESS:
				key = do_keyscan();
				if(key != 0)
				{
					e_isrState = STATE_WAIT_FOR_RELEASE;
				}
				break;

			case STATE_WAIT_FOR_RELEASE:
				//keypad released
				if(key_is_released() == 1)
				{
					e_isrState = STATE_WAIT_FOR_PRESS;
					drive_row_high();
					sendval = (unsigned long)key;
					xQueueSend(keypressedQueueHandle, &sendval, 0);
					key = 0;
				}
				break;
			default:
				e_isrState = STATE_WAIT_FOR_PRESS;
				break;
		}
		vTaskDelay(1);
	}
}
/**
* @brief read data from DS1620
* @param argument: Not used
* @retval None
*/
void StartDS1620Task(void const * argument)
{
	UINT raw_data;
	UCHAR row, col;
	uint64_t avr_buffer[5];
	UCHAR ucbuff[8];
	uint64_t temp;

	initDS1620();
	vTaskDelay(1000);

	row = col = 0;
	raw_data = 0;
	memset(ucbuff, 0, 8);

	row = 0;
	col = 30;
	for(;;)
	{
		writeByteTo1620(DS1620_CMD_STARTCONV);
		vTaskDelay(10);
		raw_data = readTempFrom1620();
		vTaskDelay(10);
		writeByteTo1620(DS1620_CMD_STOPCONV);
		vTaskDelay(100);

		ucbuff[0] = DISPLAY_TEMP;
		ucbuff[1] = row;
		ucbuff[2] = col;
		ucbuff[4] = (UCHAR)raw_data;
		raw_data >>= 8;
		ucbuff[3] = (UCHAR)raw_data;

		avr_buffer[0] = pack64(ucbuff);
		xQueueSend(SendAVRQueueHandle,avr_buffer,0);
		vTaskDelay(1000);
	
/*
		if(++row > ROWS)
		{
			row = 0;
			if((col += 8) > 37)
			{
				clr_scr();
				col = 0;
			}
		}
*/
	}
}
/**
* @brief recv msg's for 7200 and send to USART 1
* @param argument: Not used
* @retval None
*/
// queue receiving task - unpacks the param for the queue
// and transmits to 7200 while at the same time gets
// message from 7200 
void StartTask7200(void const * argument)
{
	UCHAR buff[8];
	UCHAR no_sent, no_sent2;
	uint64_t send_char[5];
	UCHAR cmd;
	UCHAR param = 0;
	UCHAR note_len = 20;
	UCHAR duty_cycle = 0x3F;
	int i,j;

	no_sent = 0x21;
	no_sent2 = 1;

//	cmd = LOAD_TUNE;
	
	for(;;)
	{
		cmd = DTMF_TONE_ON;
		buff[0] = cmd;
		buff[1] = param;
		buff[2] = note_len;
		buff[3] = duty_cycle;
		buff[4] = no_sent2+3;
		buff[5] = no_sent2+4;
		buff[6] = no_sent2+5;
		buff[7] = no_sent2+6;

		send_char[0] = pack64(buff);
		xQueueSend(SendFPGAQueueHandle, send_char, 0);
		vTaskDelay(100);

		cmd = DTMF_TONE_OFF;
		buff[0] = cmd;
		buff[1] = 1;
		buff[2] = no_sent+2;
		buff[3] = no_sent+3;
		buff[4] = no_sent+4;
		buff[5] = no_sent+5;
		buff[6] = no_sent+6;
		buff[7] = no_sent+7;
		no_sent++;
		if(no_sent > 0x7e)
			no_sent = 0x21;
		no_sent2++;
//		if(++cmd > 0x2D)
//			cmd = LOAD_TUNE;
		
		if(no_sent2 > 40)
			no_sent2 = 2;

		send_char[0] = pack64(buff);
		xQueueSend(SendFPGAQueueHandle, send_char, 0);
		vTaskDelay(1000);

		if(++param > 12)
			param = 0;

		if((note_len -= 2) < 20)
			note_len = 40;

		HAL_UART_Transmit(&huart1, &param, 1, 100);
		vTaskDelay(10);
//		HAL_UART_Transmit(&huart1, &note_len, 1, 100);
		vTaskDelay(10);

		if(--duty_cycle < 0x02)
			duty_cycle = 0x1F;

	}
}
/**
* @brief get msg's to be sent to AVR (LCD screen)
* @param argument: Not used
* @retval None
*/
void StartAVRTask(void const * argument)
{
//	TickType_t delay = 200;
	uint64_t avr_buffer[5];
	UCHAR ucbuff[8];
	UCHAR end_byte;
	uint64_t temp;

	for(;;)
	{
		xQueueReceive(SendAVRQueueHandle, avr_buffer, portMAX_DELAY);

		ucbuff[0] = (UCHAR)avr_buffer[0];
		avr_buffer[0] >>= 8;

		ucbuff[1] = (UCHAR)avr_buffer[0];
		avr_buffer[0] >>= 8;

		ucbuff[2] = (UCHAR)avr_buffer[0];
		avr_buffer[0] >>= 8;

		ucbuff[3] = (UCHAR)avr_buffer[0];
		avr_buffer[0] >>= 8;

		ucbuff[4] = (UCHAR)avr_buffer[0];
		avr_buffer[0] >>= 8;

		ucbuff[5] = (UCHAR)avr_buffer[0];
		avr_buffer[0] >>= 8;

		ucbuff[6] = (UCHAR)avr_buffer[0];
		avr_buffer[0] >>= 8;

		ucbuff[7] = (UCHAR)avr_buffer[0];
		avr_buffer[0] >>= 8;

		switch (ucbuff[0])
		{
			case EEPROM_STR:
			case EEPROM_STR2:
			case DISPLAY_TEMP:
			case SEND_INT_RT_VALUES:
			case DISPLAY_ELAPSED_TIME:
			case SET_MODE_CMD:
				HAL_UART_Transmit(&huart2, ucbuff, 5, 100);
				break;
			case CHAR_CMD:
			case SET_NUM_ENTRY_MODE:
			case PASSWORD_MODE:
				HAL_UART_Transmit(&huart2, ucbuff, 2, 100);
				break;
			case GOTO_CMD:
				HAL_UART_Transmit(&huart2, ucbuff, 3, 100);
				break;
			case DISPLAY_RTLABELS:
				HAL_UART_Transmit(&huart2, ucbuff, 6, 100);
				break;
			case SEND_BYTE_HEX_VALUES:
			case SEND_BYTE_RT_VALUES:
				HAL_UART_Transmit(&huart2, ucbuff, 4, 100);
				break;
			case LCD_CLRSCR:
			case DISPLAY_STATUSLABELS:
				HAL_UART_Transmit(&huart2, ucbuff, 1, 100);
				break;
			default:
				break;
		}
		end_byte = 0xFE;
		HAL_UART_Transmit(&huart2, &end_byte, 1, 100);
		end_byte = 0;
		do {
			HAL_UART_Receive(&huart2, &end_byte, 1, portMAX_DELAY);
		} while(end_byte != 0xFD);
	}
}
/**
* @brief get cmd's from 7200 and process: mostly display on screen
* @param argument: Not used
* @retval None
*/
void StartRecv7200(void const * argument)
{
	/* USER CODE BEGIN StartRecv7200 */
	UCHAR rec_byte;
	uint64_t avr_buffer[5];
	UCHAR ucbuff[8];
	UCHAR onoff;
	int i;
	HAL_StatusTypeDef uart_rec;
//	UCHAR xbyte;

//	goto_scr(0,0);
	/* Infinite loop */

	for(;;)
	vTaskDelay(3000);

	/* USER CODE END StartRecv7200 */
}
/**
* @brief send cmd's to FPGA over USART3
* @param argument: Not used
* @retval None
*/
void StartSendFPGA(void const * argument)
{
	/* USER CODE BEGIN StartSendFPGA */
	// send data to the FPGA and use handshaking lines DataReady & CmdParam
	// 1) set CmdParam
	// 2) set DataReady
	// 3) send the cmd
	// 4) set both lines low
	// 5) set DataReady high
	// 6) send the param
	// 7) set DataReady low

	UCHAR cmd, param1, param2, param3, param4, param5, param6, param7;
	uint64_t rec_val[5];

	/* Infinite loop */
	menu_ptr = 0;
	for(;;)
	{
		xQueueReceive(SendFPGAQueueHandle, rec_val, portMAX_DELAY);

		cmd = (UCHAR)rec_val[0];
		rec_val[0] >>= 8;

		param1 = (UCHAR)rec_val[0];
		rec_val[0] >>= 8;

		param2 = (UCHAR)rec_val[0];
		rec_val[0] >>= 8;

		param3 = (UCHAR)rec_val[0];
		rec_val[0] >>= 8;

		param4 = (UCHAR)rec_val[0];
		rec_val[0] >>= 8;

		param5 = (UCHAR)rec_val[0];
		rec_val[0] >>= 8;

		param6 = (UCHAR)rec_val[0];
		rec_val[0] >>= 8;

		param7 = (UCHAR)rec_val[0];

		HAL_GPIO_WritePin(GPIOC, CmdParam_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_SET);
		SEND_FPGA_DELAY();
		HAL_UART_Transmit(&huart3, &cmd, 1, 100);

		SEND_FPGA_DELAY();
		HAL_GPIO_WritePin(GPIOC, CmdParam_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_RESET);
		SEND_FPGA_DELAY();

		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_SET);
		SEND_FPGA_DELAY();
		HAL_UART_Transmit(&huart3, &param1, 1, 100);
		SEND_FPGA_DELAY();
		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_RESET);
		SEND_FPGA_DELAY();

		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_SET);
		SEND_FPGA_DELAY();
		HAL_UART_Transmit(&huart3, &param2, 1, 100);
		SEND_FPGA_DELAY();
		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_RESET);
		SEND_FPGA_DELAY();

		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_SET);
		SEND_FPGA_DELAY();
		HAL_UART_Transmit(&huart3, &param3, 1, 100);
		SEND_FPGA_DELAY();
		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_SET);
		SEND_FPGA_DELAY();
		HAL_UART_Transmit(&huart3, &param4, 1, 100);
		SEND_FPGA_DELAY();
		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_SET);
		SEND_FPGA_DELAY();
		HAL_UART_Transmit(&huart3, &param5, 1, 100);
		SEND_FPGA_DELAY();
		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_SET);
		SEND_FPGA_DELAY();
		HAL_UART_Transmit(&huart3, &param6, 1, 100);
		SEND_FPGA_DELAY();
		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_SET);
		SEND_FPGA_DELAY();
		HAL_UART_Transmit(&huart3, &param7, 1, 100);
		SEND_FPGA_DELAY();
		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_RESET);

		param7 = 0xFF;
		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_SET);
		SEND_FPGA_DELAY();
		HAL_UART_Transmit(&huart3, &param7, 1, 100);
		SEND_FPGA_DELAY();
		HAL_GPIO_WritePin(GPIOC, DataReady_Pin, GPIO_PIN_RESET);
#if 0
		if(menu_ptr == 0)
		{
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
			menu_ptr = 1;
		}else
		{
			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
			menu_ptr = 0;
		}
#endif
	}
	/* USER CODE END StartSendFPGA */
}
/**
* @brief recv data from FPGA
* @param argument: Not used
* @retval None
*/
void StartRecvFPGA(void const * argument)
{
	UCHAR xbyte = 0;
	int i;
	GPIO_PinState state;

	UCHAR buff[100];
	
	xbyte = 0x21;

//	HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
//	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);

	vTaskDelay(2000);
	HAL_GPIO_WritePin(GPIOA, PP_ACK, GPIO_PIN_SET);
	vTaskDelay(10);
	HAL_GPIO_WritePin(GPIOA, PP_ACK, GPIO_PIN_RESET);
	vTaskDelay(1);
	//osTimerStart(myTimer01Handle,500);
	
	for(;;)
	{
		do {
			state = HAL_GPIO_ReadPin(GPIOA,PP_CS);
		}while(state != GPIO_PIN_SET);

//		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
			
		state = HAL_GPIO_ReadPin(GPIOB,PP0);
		if(state == GPIO_PIN_SET)
			xbyte |= 1;
		state = HAL_GPIO_ReadPin(GPIOB,PP1);
		if(state == GPIO_PIN_SET)
			xbyte |= 0x2;
		state = HAL_GPIO_ReadPin(GPIOB,PP2);
		if(state == GPIO_PIN_SET)
			xbyte |= 0x4;
		state = HAL_GPIO_ReadPin(GPIOB,PP3);
		if(state == GPIO_PIN_SET)
			xbyte |= 0x8;
		state = HAL_GPIO_ReadPin(GPIOB,PP4);
		if(state == GPIO_PIN_SET)
			xbyte |= 0x10;
		state = HAL_GPIO_ReadPin(GPIOB,PP5);
		if(state == GPIO_PIN_SET)
			xbyte |= 0x20;
		state = HAL_GPIO_ReadPin(GPIOB,PP6);
		if(state == GPIO_PIN_SET)
			xbyte |= 0x40;
		state = HAL_GPIO_ReadPin(GPIOA,PP7);
		if(state == GPIO_PIN_SET)
			xbyte |= 0x80;

		do {
			state = HAL_GPIO_ReadPin(GPIOA,PP_CS);
		}while(state != GPIO_PIN_RESET);

//		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
//		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

//		HAL_UART_Transmit(&huart1, &xbyte, 1, 100);
		HAL_GPIO_WritePin(GPIOA, PP_ACK, GPIO_PIN_SET);
		vTaskDelay(5);
		HAL_GPIO_WritePin(GPIOA, PP_ACK, GPIO_PIN_RESET);
		xbyte = 0;
	}
/* USER CODE END StartRecvFPGA */
}
void StartTask10(void const * argument)
{
	UCHAR xbuff[100];
	UCHAR xbyte = 0x21;
	int i,k;
	uint64_t avr_buffer[5];
	UCHAR ucbuff[8];
	
	for(i = 0;i < 100;i++)
	{
		xbuff[i] = xbyte;
		if(++xbyte > 0x7e)
			xbyte = 0x21;
	}
	HAL_UART_Transmit(&huart1, xbuff, 100, 100);
	vTaskDelay(3000);
	ucbuff[0] = SET_MODE_CMD;
	ucbuff[1] = TEXT_ON | CURSOR_ON_BLINK_OFF; 
	ucbuff[2] = 1;
	ucbuff[3] = 1;
	ucbuff[4] = LINE_8_CURSOR;
	avr_buffer[0] = pack64(ucbuff);
	xQueueSend(SendAVRQueueHandle,avr_buffer,0);
	vTaskDelay(10);
	ucbuff[0] = CHAR_CMD;
	xbyte = 0x21;
	goto_scr(0,0);

	i = 0;
	k = 0;
	for(;;)
	{
		HAL_UART_Transmit(&huart1, xbuff, 100, 100);
		if(++xbyte > 0x7e)
		{
			xbyte = 0x21;

			if(++i > 2)
			{
				i = 0;
				ucbuff[0] = SET_MODE_CMD;
				if((k++ % 2) == 0)
					ucbuff[1] = TEXT_ON | CURSOR_ON_BLINK_OFF;
				else
					ucbuff[1] = TEXT_ON | CURSOR_BLINK_ON;
				ucbuff[2] = k;
				ucbuff[3] = k+2;
				ucbuff[4] = LINE_8_CURSOR;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				vTaskDelay(100);
				if(k > 15)
					k = 0;
			}
		}
		ucbuff[0] = CHAR_CMD;
		ucbuff[1] = xbyte;
		avr_buffer[0] = pack64(ucbuff);
		xQueueSend(SendAVRQueueHandle,avr_buffer,0);
		vTaskDelay(10);
//		HAL_UART_Transmit(&huart1, &xbyte, 1, 100);

		vTaskDelay(300);
/*
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,GPIO_PIN_SET);
		vTaskDelay(50);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,GPIO_PIN_RESET);
*/
#if 0
		if(menu_ptr == 0)
		{
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
			menu_ptr = 1;
		}else
		{
			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
			menu_ptr = 0;
		}
#endif
	}
}
/**
* @brief  (available)
* @param argument: Not used
* @retval None
*/
void StartNumEntryTask(void const * argument)
{

	uint64_t avr_buffer[5];
	uint16_t ukey;
	UCHAR ucbuff[8];
	UCHAR data1;
	int i;
	UCHAR num_entry_ptr;
	UCHAR curr_num_entry_row;
	UCHAR curr_num_entry_col;
	UCHAR num_entry_limit;
	UCHAR num_entry_type;

	memset(num_entry_num,0,SIZE_NUM);
	num_entry_ptr = 0;

	curr_num_entry_col = 20;
	curr_num_entry_row = 0;
	num_entry_type = TIME_DATE;
	num_entry_limit = SIZE_NUM;

	vTaskDelay(1000);
	ucbuff[0] = CHAR_CMD;
	ucbuff[1] = 0x20;
	avr_buffer[0] = pack64(ucbuff);
	xQueueSend(SendAVRQueueHandle,avr_buffer,0);
	vTaskDelay(20);

	ucbuff[0] = GOTO_CMD;
	ucbuff[1] = (UCHAR)curr_num_entry_row;
	ucbuff[2] = (UCHAR)curr_num_entry_col;
	avr_buffer[0] = pack64(ucbuff);
	xQueueSend(SendAVRQueueHandle,avr_buffer,0);

	for(;;)
	{
		xQueueReceive(keypressedQueueHandle, &ukey, portMAX_DELAY);
		data1 = (UCHAR)ukey;
		switch(data1)
		{
			case 	KP_1:
			case	KP_2:
			case	KP_3:
			case	KP_4:
			case	KP_5:
			case	KP_6:
			case	KP_7:
			case	KP_8:
			case	KP_9:
			case	KP_0:

				if(data1 == KP_0)
					data1 = data1 - KP_0;
				else	
					data1 = data1 - KP_1 + 1;

				num_entry_num[num_entry_ptr] = data1;
				num_entry_ptr++;

				if(num_entry_ptr > num_entry_limit)
				{
					num_entry_ptr = num_entry_limit;
				}

				ucbuff[0] = SET_MODE_CMD;
				ucbuff[1] = TEXT_ON | CURSOR_BLINK_ON;
				ucbuff[2] = curr_num_entry_row;
				ucbuff[3] = curr_num_entry_col;
				ucbuff[4] = LINE_8_CURSOR;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				vTaskDelay(100);

				ucbuff[0] = GOTO_CMD;
				ucbuff[1] = curr_num_entry_row;
				ucbuff[2] = curr_num_entry_col;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				vTaskDelay(100);

				ucbuff[0] = CHAR_CMD;
				ucbuff[1] = data1 + 0x30;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				curr_num_entry_col++;
//				HAL_UART_Transmit(&huart1, &data1, 1, 100);
			break;

			// 'backspace'
			case	KP_B:
				ucbuff[0] = GOTO_CMD;
				ucbuff[1] = curr_num_entry_row;
				--curr_num_entry_col;
				ucbuff[2] = curr_num_entry_col;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				vTaskDelay(10);

				ucbuff[0] = CHAR_CMD;
				ucbuff[1] = 0x20;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				vTaskDelay(10);

				ucbuff[0] = SET_MODE_CMD;
				ucbuff[1] = TEXT_ON | CURSOR_BLINK_ON;
				ucbuff[2] = curr_num_entry_row;
				ucbuff[3] = curr_num_entry_col;
				ucbuff[4] = LINE_8_CURSOR;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				vTaskDelay(10);

				if(--num_entry_ptr < 0)
					num_entry_ptr = 0;


			break;
			// clear entry and start over
			case	KP_A:
			case	KP_C:
				curr_num_entry_col -= num_entry_ptr;
				ucbuff[0] = GOTO_CMD;
				ucbuff[1] = curr_num_entry_row;
				ucbuff[2] = curr_num_entry_col;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				vTaskDelay(10);
				for(i = 0;i < num_entry_ptr;i++)
				{
					ucbuff[0] = CHAR_CMD;
					ucbuff[1] = 0x20;
					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(SendAVRQueueHandle,avr_buffer,0);
					vTaskDelay(10);
				}
				ucbuff[0] = GOTO_CMD;
				ucbuff[1] = curr_num_entry_row;
				ucbuff[2] = curr_num_entry_col;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				vTaskDelay(10);
				ucbuff[0] = SET_MODE_CMD;
				ucbuff[1] = TEXT_ON | CURSOR_BLINK_ON;
				ucbuff[2] = curr_num_entry_row;
				ucbuff[3] = curr_num_entry_col;
				ucbuff[4] = LINE_8_CURSOR;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				vTaskDelay(10);

				if(data1 == KP_A)
				{
					ucbuff[0] = GOTO_CMD;
					ucbuff[1] = 0;
					ucbuff[2] = 0;
					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(SendAVRQueueHandle,avr_buffer,0);
					vTaskDelay(100);

					ucbuff[0] = CHAR_CMD;
					for(i = 0;i < num_entry_ptr;i++)
					{
						ucbuff[1] = num_entry_num[i] + 0x30;
						avr_buffer[0] = pack64(ucbuff);
						xQueueSend(SendAVRQueueHandle,avr_buffer,0);
						vTaskDelay(10);
					}
					key_mode = NORMAL;
					// now do something with the number just entered in num_entry_num
					for(i = 0;i < num_entry_ptr;i++)
						HAL_UART_Transmit(&huart1, &num_entry_num[i], 1, 100);

					switch(num_entry_type)
					{
						case TIME_DATE:
						break;
						case TIME_ENG_OFF:
						break;
						case FPGA_SEND_RATE:
						break;
						case RPM_MPH_UPDATE_RATE:
						break;
						case HIGH_REV_LIMIT:
						break;
						case LOW_REV_LIMIT:
						break;
						case SET_LIGHTS_OFF:
						break;
						case CHANGE_PASSWORD:
						break;
						default:
						break;
					}
				}
				memset(num_entry_num,0,SIZE_NUM);
				num_entry_ptr = 0;

			break;
			case	KP_D:
				num_entry_ptr = 0;
				memset(num_entry_num,0,SIZE_NUM);
				key_mode = NORMAL;
			break;
			case	KP_POUND:

				clr_scr();
				vTaskDelay(10);
				goto_scr(0,0);
				vTaskDelay(10);
				for(i = 0;i < num_entry_ptr;i++)
				{
					ucbuff[0] = CHAR_CMD;
					ucbuff[1] = num_entry_num[i] + 0x30;
					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(SendAVRQueueHandle,avr_buffer,0);
					vTaskDelay(10);
					HAL_UART_Transmit(&huart1, &num_entry_num[i], 1, 100);
					vTaskDelay(10);
				}

			break;
			case	KP_AST:
			break;
			default:
			break;
		}
	}
}
void Callback01(void const * argument)
{
//#if 0
	if(menu_ptr == 0)
	{
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
		menu_ptr = 1;
	}else
	{
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
		menu_ptr = 0;
	}
//#endif
}
/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
uint64_t pack64(UCHAR *buff)
{
	uint64_t var64;
	UCHAR temp;

	var64 = (uint64_t)buff[7];
	var64 <<= 8;
	var64 |= (uint64_t)buff[6];
	var64 <<= 8;
	var64 |= (uint64_t)buff[5];
	var64 <<= 8;
	var64 |= (uint64_t)buff[4];
	var64 <<= 8;
	var64 |= (uint64_t)buff[3];
	var64 <<= 8;
	var64 |= (uint64_t)buff[2];
	var64 <<= 8;
	var64 |= (uint64_t)buff[1];
	var64 <<= 8;
	var64 |= (uint64_t)buff[0];
	return var64;
}
uint32_t pack32(UCHAR *buff)
{
	uint32_t var32;
	UCHAR temp;

	var32 = 0L;
	var32 |= (uint32_t)buff[3];
	var32 <<= 8;
	var32 |= (uint32_t)buff[2];
	var32 <<= 8;
	var32 |= (uint32_t)buff[1];
	var32 <<= 8;
	var32 |= (uint32_t)buff[0];

	return var32;
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint16_t temp;
	UCHAR uTemp;
	temp = GPIO_Pin;
	uTemp = (UCHAR)temp;
	temp >>= 8;
	uTemp = (UCHAR)temp;
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
#if 0
	if(menu_ptr == 0)
	{
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
		menu_ptr = 1;
	}else
	{
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
		menu_ptr = 0;
	}
#endif
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

