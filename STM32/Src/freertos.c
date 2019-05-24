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
//#include "adc.h"
//#include "raw_data.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
	STATE_WAIT_FOR_PRESS = 1,
	STATE_WAIT_FOR_RELEASE
} ISRSTATE;

static ISRSTATE e_isrState;

const uint8_t au8_keyTable[NUM_ROWS][NUM_COLS] =
{
	{KP_1, KP_2, KP_3, KP_A },
	{KP_4, KP_5, KP_6, KP_B },
	{KP_7, KP_8, KP_9, KP_C },
	{KP_AST, KP_0, KP_POUND, KP_D}
};

FORMAT_STR rtlabel_str[NUM_RT_LABELS];
FORMAT_STR status_label_str[NUM_STATUS_LABELS];

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint64_t pack64(UCHAR *buff);
uint32_t pack32(UCHAR *buff);

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
static int key_enter_time;
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
static UCHAR gl_comm1_buf_len;
static int engine_run_time;
static int engine_shutdown;
static int engine_shuttingdown;
static char num_entry_num[SIZE_NUM];
static int task7on;
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
uint8_t myQueue02Buffer[ 16 * sizeof( uint16_t ) ];
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
	osMessageQStaticDef(myQueue02, 16, uint16_t, myQueue02Buffer, &myQueue02ControlBlock);
	Send7200QueueHandle = osMessageCreate(osMessageQ(myQueue02), NULL);

	/* definition and creation of myQueue03 */
	osMessageQStaticDef(myQueue03, 5, uint64_t, myQueue03Buffer, &myQueue03ControlBlock);
	SendAVRQueueHandle = osMessageCreate(osMessageQ(myQueue03), NULL);

	/* definition and creation of myQueue03 */
	osMessageQStaticDef(myQueue04, 5, uint64_t, myQueue04Buffer, &myQueue04ControlBlock);
	SendFPGAQueueHandle = osMessageCreate(osMessageQ(myQueue04), NULL);
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadStaticDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 70, defaultTaskBuffer,
		&defaultTaskControlBlock);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask02 */
  //StartBasicCmdTask
  osThreadStaticDef(BasicCmdTask, StartBasicCmdTask, osPriorityIdle, 0, 70, myTask02Buffer, 
         &myTask02ControlBlock);
  BasicCmdTaskHandle = osThreadCreate(osThread(BasicCmdTask), NULL);

  /* definition and creation of myTask03 */
  // StartKeyStateTask
  osThreadStaticDef(myTask03, StartKeyStateTask, osPriorityIdle, 0, 70, myTask03Buffer, 
		&myTask03ControlBlock);
  KeyStateTaskHandle = osThreadCreate(osThread(myTask03), NULL);
#if 0
  /* definition and creation of myTask04 */
  osThreadStaticDef(myTask04, StartDS1620Task, osPriorityIdle, 0, 70, myTask04Buffer, 
		&myTask04ControlBlock);
  DS1620TaskHandle = osThreadCreate(osThread(myTask04), NULL);
#endif
  /* definition and creation of myTask05 */
  osThreadStaticDef(myTask05, StartTask7200, osPriorityIdle, 0, 70, myTask05Buffer, &myTask05ControlBlock);
  Task7200Handle = osThreadCreate(osThread(myTask05), NULL);

  /* definition and creation of myTask06 */
  osThreadStaticDef(myTask06, StartAVRTask, osPriorityIdle, 0, 70, myTask06Buffer, &myTask06ControlBlock);
  AVRTaskHandle = osThreadCreate(osThread(myTask06), NULL);
#if 0
  /* definition and creation of myTask07 */
  osThreadStaticDef(myTask07, StartRecv7200, osPriorityIdle, 0, 70, myTask07Buffer, &myTask07ControlBlock);
  myTask07Handle = osThreadCreate(osThread(myTask07), NULL);

  /* definition and creation of myTask08 */
  osThreadStaticDef(myTask08, StartSendFPGA, osPriorityIdle, 0, 70, myTask08Buffer, &myTask08ControlBlock);
  myTask08Handle = osThreadCreate(osThread(myTask08), NULL);

// disable the last 2 tasks or it hangs - not enough memory, I guess
  /* definition and creation of myTask09 */
  osThreadStaticDef(myTask09, StartRecvFPGA, osPriorityIdle, 0, 70, myTask09Buffer, &myTask09ControlBlock);
  myTask09Handle = osThreadCreate(osThread(myTask09), NULL);

  /* definition and creation of myTask10 */
  osThreadStaticDef(myTask10, StartTask10, osPriorityIdle, 0, 70, myTask10Buffer, &myTask10ControlBlock);
  myTask10Handle = osThreadCreate(osThread(myTask10), NULL);

  /* definition and creation of myTask11 */
  osThreadStaticDef(myTask11, StartNumEntryTask, osPriorityIdle, 0, 70, myTask11Buffer, &myTask11ControlBlock);
  myTask11Handle = osThreadCreate(osThread(myTask11), NULL);
#endif

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
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

//	key_mode = PASSWORD;
	key_mode = NORMAL;
	DWT_Delay_Init();
//	osTimerStart(myTimer01Handle,500);

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
//	osTimerStart(myTimer01Handle,1000);

	for(;;)
	{
		vTaskDelay(2000);
	}
	/* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief get keypresses and process - mostly sending msg to 7200 task
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartBasicCmdTask(void const * argument)
{
	UCHAR key;
	UCHAR cmd;
	uint16_t recval;
	uint16_t sendval; 
		
	lights_on = 0;
	brights_on = 0;
	fan_on = 0;
	blower_on = 0;
	engine_on = 0;
	task7on = 0;
	wipers = 0;

	sendval = 8;

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
					sendval = (uint16_t)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
					HAL_GPIO_WritePin(LD3_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
				}
			break;
			case KP_2:
				cmd = SHUTDOWN;
				sendval = (uint16_t)cmd;
				xQueueSend(Send7200QueueHandle, &sendval, 0);
				cmd = OFF_FAN;
				sendval = (uint16_t)cmd;
				xQueueSend(Send7200QueueHandle, &sendval, 0);
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
						sendval = (uint16_t)cmd;
						xQueueSend(Send7200QueueHandle, &sendval, 0);
						brights_on = 1;
					}
					else
					{
						cmd = OFF_BRIGHTS;
						sendval = (uint16_t)cmd;
						xQueueSend(Send7200QueueHandle, &sendval, 0);
						brights_on = 0;
					}
				}
			break;
			case KP_4:
				if(fan_on == 0)
				{
					cmd = ON_FAN;
					sendval = (uint16_t)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
					fan_on = 1;
					HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
				}
				else
				{
					cmd = OFF_FAN;
					sendval = (uint16_t)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
					fan_on = 0;
					HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
				}
			break;
			case KP_5:
				switch(blower_on)
				{
					case 0:	
					cmd = BLOWER_OFF;
					sendval = (uint16_t)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
					break;
					case 1:
					cmd = BLOWER1;
					sendval = (uint16_t)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
					break;
					case 2:
					cmd = BLOWER2;
					sendval = (uint16_t)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
					break;
					case 3:
					cmd = BLOWER3;
					sendval = (uint16_t)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
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
					sendval = (uint16_t)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
				}else
				{
					running_lights_on = 0;
					cmd = OFF_RUNNING_LIGHTS;
					sendval = (uint16_t)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
				}
			break;
			case KP_7:
				if(lights_on == 0)
				{
					cmd = ON_LIGHTS;
					sendval = (uint16_t)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
					lights_on = 1;
					sendval = 4;
//					xQueueSend(FlashQueueHandle, &sendval, 0);
				}
				else
				{
					cmd = OFF_LIGHTS;
					lights_on = 0;
					sendval = (uint16_t)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
					sendval = 2;
//					xQueueSend(FlashQueueHandle, &sendval, 0);
					if(brights_on == 1)
					{
						cmd = OFF_BRIGHTS;
						sendval = (uint16_t)cmd;
						xQueueSend(Send7200QueueHandle, &sendval, 0);
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
				sendval = (uint16_t)cmd;
				xQueueSend(Send7200QueueHandle, &sendval, 0);
			break;
			case KP_9:
			break;
		}
		// send to USART1 for testing
		switch(key)
		{
			case KP_0:
				key = '0';
			break;
			case KP_1:
				key = '1';
			break;
			case KP_2:
				key = '2';
			break;
			case KP_3:
				key = '3';
			break;
			case KP_4:
				key = '4';
			break;
			case KP_5:
				key = '5';
			break;
			case KP_6:
				key = '6';
			break;
			case KP_7:
				key = '7';
			break;
			case KP_8:
				key = '8';
			break;
			case KP_9:
				key = '9';
			break;
			case KP_A:
				key = 'A';
			break;
			case KP_B:
				key = 'B';
			break;
			case KP_C:
				key = 'C';
			break;
			case KP_D:
				key = 'D';
			break;
		}

		vTaskDelay(10);
	}
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief state machine for keypad
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
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

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief read data from DS1620
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartDS1620Task(void const * argument)
{
	UINT raw_data;
	UCHAR row, col;
	uint64_t avr_buffer[5];
	UCHAR ucbuff[8];
	UCHAR xbyte;
	uint64_t temp;

	initDS1620();
	vTaskDelay(1000);

	row = col = 0;
	raw_data = 0;
	memset(ucbuff, 0, 8);
	xbyte = 0x21;

	ucbuff[0] = CHAR_CMD;

/*
	for(row = 0;row < ROWS;row++)
		for(col = 0;col < COLUMN;col++)
		{
			ucbuff[1] = xbyte;
			avr_buffer[0] = pack64(ucbuff);
			xQueueSend(SendAVRQueueHandle,avr_buffer,0);
			vTaskDelay(5);
			if(++xbyte > 0x7e)
				xbyte = 0x21;
		}
*/
	clr_scr();

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
/*	
		ucbuff[0] = CHAR_CMD;
		ucbuff[1] = xbyte;
		avr_buffer[0] = pack64(ucbuff);
*/
		xQueueSend(SendAVRQueueHandle,avr_buffer,0);
		vTaskDelay(1000);
		if(++xbyte > 0x7e)
			xbyte = 0x21;
	
		if(++row > ROWS)
		{
			row = 0;
			if((col += 8) > 37)
			{
				clr_scr();
				col = 0;
			}
		}
	}
}

/* USER CODE BEGIN Header_StartTask05 */
/**
* @brief recv msg's for 7200 and send to USART 1
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask05 */
void StartTask7200(void const * argument)
{
	unsigned long ulReceivedValue;
	UCHAR cmd;
	UCHAR ucbuff[4];
	uint64_t avr_buffer[5];
	UCHAR rec_byte;

	for(;;)
	{
		xQueueReceive(Send7200QueueHandle, &ulReceivedValue, portMAX_DELAY);
		cmd = (UCHAR)ulReceivedValue;
		HAL_UART_Transmit(&huart1, &cmd, 1, 100);
#if 0
		HAL_UART_Receive(&huart1, &rec_byte, 1, portMAX_DELAY);
		ucbuff[0] = SEND_BYTE_HEX_VALUES;
		ucbuff[1] = 0;
		ucbuff[2] = 30;
		ucbuff[3] = rec_byte;
		avr_buffer[0] = pack64(ucbuff);
		xQueueSend(SendAVRQueueHandle,avr_buffer,0);
#endif
	}
}

/* USER CODE BEGIN Header_StartTask06 */
/**
* @brief get msg's to be sent to AVR (LCD screen)
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask06 */
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

/* USER CODE BEGIN Header_StartRecv7200 */
/**
* @brief get cmd's from 7200 and process: mostly display on screen
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartRecv7200 */
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

#if 0	

	onoff = 1;
	ucbuff[0] = EEPROM_STR2;

	rec_byte = SHUTDOWN;
	engine_on = 0;
	ucbuff[1] = rec_byte;
	ucbuff[2] = 1;
	avr_buffer[0] = pack64(ucbuff);
	xQueueSend(SendAVRQueueHandle,avr_buffer,0);
	vTaskDelay(100);

	rec_byte = BLOWER_OFF;
	ucbuff[1] = rec_byte;
	ucbuff[2] = 1;
	avr_buffer[0] = pack64(ucbuff);
	xQueueSend(SendAVRQueueHandle,avr_buffer,0);
	vTaskDelay(100);

	rec_byte = OFF_FAN;
	ucbuff[1] = rec_byte;
	ucbuff[2] = 1;
	avr_buffer[0] = pack64(ucbuff);
	xQueueSend(SendAVRQueueHandle,avr_buffer,0);
	vTaskDelay(100);

	rec_byte = OFF_LIGHTS;
	lights_on = 0;
	ucbuff[1] = rec_byte;
	ucbuff[2] = 1;
	avr_buffer[0] = pack64(ucbuff);
	xQueueSend(SendAVRQueueHandle,avr_buffer,0);
	vTaskDelay(100);

	rec_byte = OFF_RUNNING_LIGHTS;
	ucbuff[1] = rec_byte;
	ucbuff[2] = 1;
	avr_buffer[0] = pack64(ucbuff);
	xQueueSend(SendAVRQueueHandle,avr_buffer,0);
	vTaskDelay(100);

	rec_byte = OFF_BRIGHTS;
	ucbuff[1] = rec_byte;
	ucbuff[2] = 1;
	avr_buffer[0] = pack64(ucbuff);
	xQueueSend(SendAVRQueueHandle,avr_buffer,0);
	vTaskDelay(100);

	rec_byte = OFF_BRAKES;
	ucbuff[1] = rec_byte;
	ucbuff[2] = 1;
	avr_buffer[0] = pack64(ucbuff);
	xQueueSend(SendAVRQueueHandle,avr_buffer,0);
	vTaskDelay(5000);

	for(;;)
	{
		rec_byte = 0;
		// for some reason this just doesn't work
		// it works when calling imm after and transmit in the 
		// StartTask7200 but what good is it?
		uart_rec = HAL_UART_Receive(&huart1, &rec_byte, 1, portMAX_DELAY);
		vTaskDelay(100);
/*
		ucbuff[0] = SEND_BYTE_HEX_VALUES;
		ucbuff[1] = 0;
		ucbuff[2] = 30;
		ucbuff[3] = rec_byte;
		avr_buffer[0] = pack64(ucbuff);
		xQueueSend(SendAVRQueueHandle,avr_buffer,0);
		vTaskDelay(100);
*/
		if(uart_rec == HAL_OK)
		{
			switch(rec_byte)
			{
				case START_SEQ:
					onoff = 0;
					engine_on = 1;
					break;
				case SHUTDOWN:
					onoff = 1;
					engine_on = 0;
					break;
				case BLOWER_OFF:
					onoff = 1;
					break;
				case BLOWER1:
					onoff = 2;
					break;
				case BLOWER2:
					onoff = 3;
					break;
				case BLOWER3:
					onoff = 4;
					break;
				case ON_FAN:
					onoff = 0;
					break;
				case OFF_FAN:
					onoff = 1;
					break;
				case ON_LIGHTS:
					onoff = 0;
					lights_on = 1;
					break;
				case OFF_LIGHTS:
					onoff = 1;
					lights_on = 0;
					break;
				case ON_RUNNING_LIGHTS:
					onoff = 0;
					break;
				case OFF_RUNNING_LIGHTS:
					onoff = 1;
					break;
				case ON_BRIGHTS:
					onoff = 0;
					break;
				case OFF_BRIGHTS:
					onoff = 1;
					break;
				case ON_BRAKES:
					onoff = 0;
					break;
				case OFF_BRAKES:
					onoff = 1;
					break;
				case WIPER1:
				case WIPER2:
				case WIPER_OFF:
					break;
				default:
					break;
			}	// end of switch(cmd)

			ucbuff[0] = EEPROM_STR2;
//			ucbuff[0] = CHAR_CMD;
			ucbuff[1] = rec_byte;
			ucbuff[2] = onoff;
			avr_buffer[0] = pack64(ucbuff);
			xQueueSend(SendAVRQueueHandle,avr_buffer,0);
//			printHexByte1(rec_byte);
		} else
		{
			ucbuff[0] = SEND_BYTE_HEX_VALUES;
			ucbuff[1] = 0;
			ucbuff[2] = 30;
			avr_buffer[0] = pack64(ucbuff);
			xQueueSend(SendAVRQueueHandle,avr_buffer,0);
		}
	}
#endif
	/* USER CODE END StartRecv7200 */
}

/* USER CODE BEGIN Header_StartSendFPGA */
/**
* @brief send cmd's to FPGA over USART3
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSendFPGA */
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

		HAL_GPIO_WritePin(CmdParam_GPIO_Port, CmdParam_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_SET);
		vTaskDelay(1);
		HAL_UART_Transmit(&huart3, &cmd, 1, 100);

		vTaskDelay(1);
		HAL_GPIO_WritePin(CmdParam_GPIO_Port, CmdParam_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_RESET);
		vTaskDelay(1);

		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_SET);
		vTaskDelay(1);
		HAL_UART_Transmit(&huart3, &param1, 1, 100);
		vTaskDelay(1);
		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_RESET);
		vTaskDelay(1);

		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_SET);
		vTaskDelay(1);
		HAL_UART_Transmit(&huart3, &param2, 1, 100);
		vTaskDelay(1);
		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_RESET);
		vTaskDelay(1);

		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_SET);
		vTaskDelay(1);
		HAL_UART_Transmit(&huart3, &param3, 1, 100);
		vTaskDelay(1);
		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_SET);
		vTaskDelay(1);
		HAL_UART_Transmit(&huart3, &param4, 1, 100);
		vTaskDelay(1);
		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_SET);
		vTaskDelay(1);
		HAL_UART_Transmit(&huart3, &param5, 1, 100);
		vTaskDelay(1);
		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_SET);
		vTaskDelay(1);
		HAL_UART_Transmit(&huart3, &param6, 1, 100);
		vTaskDelay(1);
		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_RESET);

		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_SET);
		vTaskDelay(1);
		HAL_UART_Transmit(&huart3, &param7, 1, 100);
		vTaskDelay(1);
		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_RESET);
	}
	/* USER CODE END StartSendFPGA */
}

/* USER CODE BEGIN Header_StartRecvFPGA */
/**
* @brief recv data from FPGA
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartRecvFPGA */
void StartRecvFPGA(void const * argument)
{
	/* USER CODE BEGIN StartRecvFPGA */
	// recv data from FPGA
	// this waits for serial input from FPGA which happens at a freq determinted by setting
	// the update rate - for now just sends the current rpm/mph preceded by '0xFF'
	// FPGA sends
	// 0 - 0xFF
	// 1 - low byte of temperature data1
	// 2 - high byte of temperature data1
	// 3 - low byte of temperature data2
	// 4 - high byte of temperature data2
	// 5 - low byte of temperature data3
	// 6 - high byte of temperature data3
	// 7 - low byte of temperature data4
	// 8 - high byte of temperature data4
	// 9 - low byte of rpm
	// 10 - high byte of mph
	// 11 - low byte of mph
	// 12 - high byte of mph
	// 13 - should be a '5'	(not used)
	// 14 - should be a '6'		"
	// 15 - should be a '7'		"

	/* Infinite loop */
	vTaskDelay(3000);
	for(;;)
	{
		vTaskDelay(500);
	}
	/* USER CODE END StartRecvFPGA */
}

/* USER CODE BEGIN Header_StartTask10 */
/**
* @brief (available)
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask10 */
void StartTask10(void const * argument)
{
	UCHAR buff[100];
	UCHAR xbyte = 0x21;
	int i;

	uint64_t avr_buffer[5];
	UCHAR ucbuff[8];
	UCHAR rec_byte;

//	osTimerStart(myTimer01Handle,1000);
	
	vTaskDelay(100);
	for(i = 0;i < 100;i++)
	{
		buff[i] = xbyte;
		if(++xbyte > 0x7e)
			xbyte = 0x21;
	}

	i = 0;
	for(;;)
	{
		vTaskDelay(500);
//		HAL_UART_Transmit(&huart1, buff, 100, 100);
//		HAL_UART_Receive(&huart1, &rec_byte, 1, portMAX_DELAY);
	}


#if 0
// use this section to test the rt display
	uint64_t avr_buffer[5];
	UCHAR ucbuff[8];
	UCHAR xbyte = 0x21;
	int i;
	UCHAR u8_pot[6];
	int j;
	
	ucbuff[0] = CHAR_CMD;
	vTaskDelay(2000);
	
	j = 0;
	for(i = 0;i < 6;i++)
		u8_pot[i] = j++;

	for(;;)
	{
		if(key_mode == NORMAL)
		{
			// engine temp doesn't come from ADC anymore
			for(i = OIL_PRES ;i <= O2;i++)
			{
				ucbuff[0] = SEND_BYTE_RT_VALUES;
				ucbuff[1] = rtlabel_str[i].row;
				ucbuff[2] = rtlabel_str[i].data_col;
				ucbuff[3] = u8_pot[i];
 				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				vTaskDelay(200);
			}
			vTaskDelay(200);

			for(i = 0;i < 6;i++)
				u8_pot[i]++;
			
			if(u8_pot[i] > 200)
			{
				for(i = 0;i < 6;i++)
					u8_pot[i] = j++;
			}
		} else vTaskDelay(1000);
	}
#endif


#if 0

// use this section to test the tone generator cmds to the FPGA
	uint32_t send_char[5];
	UCHAR mykey = 0;
	UCHAR length = 10;
	UCHAR dtmf_index = 0;
	UCHAR duty_cycle = 0;
	UCHAR temp;
	UCHAR buff[60];
	UCHAR cmd, param;
	UCHAR no_sent;
	int i,j,k;
	
	no_sent = 64;
	cmd = LOAD_TUNE;
	/* Infinite loop */

	duty_cycle = 0;

	k = 0;
	j = 0;
#if 0
	for(i = 0;i < 10;i++,j+=3)
	{
		buff[j] = (i%2==0)?k+8:k+10;
		buff[j+1] = (i%2==0)?k+14:k+16;
		buff[j+2] = 0;
		k += 2;
	}
	j = 30;
	k = 0;
	for(i = 10;i < 20;i++,j+=3)
	{
		buff[j] = (i%2==0)?44-k:40-k;
		buff[j+1] = (i%2==0)?40-k:36-k;
		buff[j+2] = 7;
		k += 2;
	}
#endif
//#if 0
	for(i = 0;i < 10;i++,j+=3)
	{
		buff[j] = k+10;
		buff[j+1] = k+14;
		buff[j+2] = 0;
		k++;
	}
	j = 30;
	k = 0;
	for(i = 10;i < 20;i++,j+=3)
	{
		buff[j] = 30-k;
		buff[j+1] = 34-k;
		buff[j+2] = 7;
		k++;
	}
//#endif
	vTaskDelay(1000);
	HAL_GPIO_WritePin(CmdParam_GPIO_Port, CmdParam_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_SET);
	vTaskDelay(10);
	HAL_UART_Transmit(&huart3, &cmd, 1, 100);
	vTaskDelay(10);
	HAL_GPIO_WritePin(CmdParam_GPIO_Port, CmdParam_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_RESET);
	vTaskDelay(10);

	HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_SET);
	vTaskDelay(10);
	HAL_UART_Transmit(&huart3, &no_sent, 1, 100);
	vTaskDelay(10);
	HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_RESET);

	for(i = 0;i < no_sent;i++)
	{
		param = buff[i];
		vTaskDelay(10);
		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_SET);
		vTaskDelay(10);
		HAL_UART_Transmit(&huart3, &param, 1, 100);
		vTaskDelay(10);
		HAL_GPIO_WritePin(DataReady_GPIO_Port, DataReady_Pin, GPIO_PIN_RESET);
		vTaskDelay(10);
	}

	length = 20;
	for(;;)
	{
#if 0
		buff[0] = DTMF_TONE_ON;
		
		temp = dtmf_index;
		temp |= 0x80;	// set the high bit for param1
		buff[1] = temp;
		buff[2] = 0;
		buff[3] = 0;
		buff[4] = 0;
		send_char[0] = pack32(buff);

		if(++dtmf_index > 14)
		{
			dtmf_index = 0;

			buff[0] = SPECIAL_TONE_ON;
			buff[1] = 0x80;
			buff[2] = 0;
			buff[3] = 0;
			send_char[0] = pack32(buff);
			xQueueSend(SendFPGAQueueHandle, send_char, 0);
			vTaskDelay(2000);

			buff[0] = DTMF_TONE_OFF;
			buff[1] = 0x80;
			buff[2] = 0;
			buff[3] = 0;
			send_char[0] = pack32(buff);
			xQueueSend(SendFPGAQueueHandle, &send_char, 0);
			vTaskDelay(100);

		}

		xQueueSend(SendFPGAQueueHandle, send_char, 0);
		vTaskDelay(300);

		buff[0] = DTMF_TONE_OFF;
		buff[1] = 0x80;
		buff[2] = 0;
		buff[3] = 0;
		send_char[0] = pack32(buff);
		xQueueSend(SendFPGAQueueHandle, send_char, 0);

		vTaskDelay(60);
		if(++i > 200)
		{
			i = 0;
			vTaskDelay(3000);
		}
#endif
//#endif

		vTaskDelay(5000);
		buff[0] = TUNE_ON;
		buff[1] = 4;		// 1st param is msg length 
		buff[2] = length;	// 1st actual param
		buff[3] = 0;
		
		send_char[0] = pack32(buff);
		xQueueSend(SendFPGAQueueHandle, send_char, 0);
		vTaskDelay(1500);
		buff[0] = TUNE_OFF;
		buff[1] = 0;
		buff[2] = 0;
		buff[3] = 0;
		send_char[0] = pack32(buff);
		xQueueSend(SendFPGAQueueHandle, send_char, 0);
		vTaskDelay(100);
		if(--length < 2)
			length = 20;

/*
		if(++duty_cycle > 7)
		{
			mykey = 0;
			if(--length < 2)
				length = 10;
		}
*/
		vTaskDelay(4000);
	}
#endif
	/* USER CODE END StartTask10 */
}

/* USER CODE BEGIN Header_StartNumEntryTask */
/**
* @brief  (available)
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartNumEntryTask */
void StartNumEntryTask(void const * argument)
{
	for(;;)
		vTaskDelay(2000);
	
#if 0
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
	num_entry_limit = 10;

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
				num_entry_num[num_entry_ptr++] = data1;

				if(num_entry_ptr > num_entry_limit)
				{
					num_entry_ptr = num_entry_limit;
				}
				curr_num_entry_col++;
				ucbuff[0] = SET_MODE_CMD;
				ucbuff[1] = TEXT_ON | CURSOR_BLINK_ON;
				ucbuff[2] = curr_num_entry_row;
				ucbuff[3] = curr_num_entry_col;
				ucbuff[4] = (LINE_1_CURSOR);
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				vTaskDelay(100);

				ucbuff[0] = GOTO_CMD;
				ucbuff[1] = curr_num_entry_row;
				ucbuff[2] = curr_num_entry_col;
				ucbuff[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				vTaskDelay(100);

				ucbuff[0] = CHAR_CMD;
				ucbuff[1] = data1 + 0x30;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				curr_num_entry_col++;
			break;

			// 'backspace'
			case	KP_B:
				ucbuff[0] = GOTO_CMD;
				ucbuff[1] = curr_num_entry_row;
				--curr_num_entry_col;
				ucbuff[2] = curr_num_entry_col;
				ucbuff[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				vTaskDelay(100);

				ucbuff[0] = CHAR_CMD;
				ucbuff[1] = 0x20;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				curr_num_entry_col++;
				vTaskDelay(100);

				ucbuff[0] = SET_MODE_CMD;
				ucbuff[1] = TEXT_ON | CURSOR_BLINK_ON;
				ucbuff[2] = curr_num_entry_row;
				ucbuff[3] = curr_num_entry_col;
				ucbuff[4] = (LINE_1_CURSOR);
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				vTaskDelay(100);

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
				ucbuff[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				vTaskDelay(100);
				for(i = 0;i < num_entry_ptr;i++)
				{
					ucbuff[0] = CHAR_CMD;
					ucbuff[1] = 0x20;
					ucbuff[0] = pack64(ucbuff);
					xQueueSend(SendAVRQueueHandle,avr_buffer,0);
					vTaskDelay(100);
				}
				ucbuff[0] = GOTO_CMD;
				ucbuff[1] = curr_num_entry_row;
				ucbuff[2] = curr_num_entry_col;
				ucbuff[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				vTaskDelay(100);
				ucbuff[0] = SET_MODE_CMD;
				ucbuff[1] = TEXT_ON | CURSOR_BLINK_ON;
				ucbuff[2] = curr_num_entry_row;
				ucbuff[3] = curr_num_entry_col;
				ucbuff[4] = (LINE_1_CURSOR);
				ucbuff[0] = pack64(ucbuff);
				xQueueSend(SendAVRQueueHandle,avr_buffer,0);
				vTaskDelay(100);

				if(data1 == KP_A)
				{
					ucbuff[0] = GOTO_CMD;
					ucbuff[1] = 0;
					ucbuff[2] = 0;
					ucbuff[0] = pack64(ucbuff);
					xQueueSend(SendAVRQueueHandle,avr_buffer,0);
					vTaskDelay(100);

					ucbuff[0] = CHAR_CMD;
					for(i = 0;i < num_entry_ptr;i++)
					{
						ucbuff[1] = num_entry_num[i] + 0x30;
						ucbuff[0] = pack64(ucbuff);
						xQueueSend(SendAVRQueueHandle,avr_buffer,0);
						vTaskDelay(100);
					}
					key_mode = NORMAL;
					// now do something with the number just entered in num_entry_num
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
			break;
			case	KP_AST:
			break;
			default:
			break;
		}
	}
#endif
	
#if 0
	int i,j;
	UCHAR cmd, param;
	UCHAR buff[8];
	UCHAR no_sent;
	uint64_t send_char[5];
	UCHAR note_len;
	UINT mask;
	UCHAR duty_cycle = 0x7F;
	int inc = 0;
	
	vTaskDelay(1000);
	cmd = 0;
	no_sent = 0;
	param = 3;
	i = 0;
	note_len = 0x7F;
	mask = 1;
	
	cmd = LOAD_TUNE;
	buff[0] = cmd;
	buff[1] = 0xFF;
	buff[2] = 0xFF;
	buff[3] = 0xFF;
	buff[4] = no_sent+1;
	buff[5] = no_sent+2;
	buff[6] = no_sent+3;
	buff[7] = 20;
	
	send_char[0] = pack64(buff);
	xQueueSend(SendFPGAQueueHandle, send_char, 0);
	vTaskDelay(1000);

	param = 19;
	for(;;)
	{

		if((note_len += 10) > 50)
		{
			note_len = 10;
		}

		cmd = TUNE_ON;
		buff[0] = cmd;
		buff[1] = param;	// 1st actual param
		buff[2] = note_len;
		buff[3] = duty_cycle;
//		buff[3] = 0xFF;
		buff[4] = 0xFF;
		buff[5] = 0;
		buff[6] = no_sent;
		buff[7] = 21;

		send_char[0] = pack64(buff);
		xQueueSend(SendFPGAQueueHandle, send_char, 0);
		vTaskDelay(2000);

		cmd = TUNE_OFF;
		buff[0] = cmd;
		buff[1] = 0xFF;
		buff[2] = 0xFF;
		buff[3] = no_sent+1;
		buff[4] = no_sent+2;
		buff[5] = no_sent+3;
		buff[6] = no_sent+4;
		buff[7] = no_sent+5;
		no_sent++;
		send_char[0] = pack64(buff);
		xQueueSend(SendFPGAQueueHandle, send_char, 0);
		vTaskDelay(2000);

//		if(--param < 1)
//			param = 20;
		
//		if((note_len -= 50) < 0x0f)
//			note_len = 0xFF;

		if(duty_cycle == 0)
		{
			duty_cycle = 0x1F;
		}else duty_cycle = 0;
	}
#endif
}
/* Callback01 function */
void Callback01(void const * argument)
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
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

