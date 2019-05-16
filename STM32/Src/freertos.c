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
//#include "adc.h"
//#include "raw_data.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint64_t pack64(UCHAR *buff);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

#if 1
static int lights_on;
static int brights_on;
static int blower_on;
static int fan_on;
static int engine_on;

static int running_lights_on;
static FORMAT_STR rtlabel_str[NUM_RT_LABELS];
static FORMAT_STR status_label_str[NUM_STATUS_LABELS];
static UCHAR menu_ptr;
static KEY_MODE key_mode;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
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
static int num_entry_ptr;
static int num_entry_limit;
static int num_entry_type;
static UINT curr_num_entry_row;
static UINT curr_num_entry_col;

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
#if 0
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
#endif
osMessageQId keypressedQueueHandle;
uint8_t myQueue01Buffer[ 16 * sizeof( uint16_t ) ];
osStaticMessageQDef_t myQueue01ControlBlock;

osMessageQId Send7200QueueHandle;
uint8_t myQueue02Buffer[ 16 * sizeof( uint16_t ) ];
osStaticMessageQDef_t myQueue02ControlBlock;

osMessageQId SendAVRQueueHandle;
uint8_t myQueue03Buffer[ 5 * sizeof( uint64_t ) ];
osStaticMessageQDef_t myQueue03ControlBlock;

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
void StartTask07(void const * argument);
void StartTask08(void const * argument);
void StartTask09(void const * argument);
void StartTask10(void const * argument);
void StartTask11(void const * argument);
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

  /* definition and creation of myTask04 */
  osThreadStaticDef(myTask04, StartDS1620Task, osPriorityIdle, 0, 70, myTask04Buffer, 
		&myTask04ControlBlock);
  DS1620TaskHandle = osThreadCreate(osThread(myTask04), NULL);

  /* definition and creation of myTask05 */
  osThreadStaticDef(myTask05, StartTask7200, osPriorityIdle, 0, 70, myTask05Buffer, &myTask05ControlBlock);
  Task7200Handle = osThreadCreate(osThread(myTask05), NULL);

  /* definition and creation of myTask06 */
  osThreadStaticDef(myTask06, StartAVRTask, osPriorityIdle, 0, 70, myTask06Buffer, &myTask06ControlBlock);
  AVRTaskHandle = osThreadCreate(osThread(myTask06), NULL);
#if 0
  /* definition and creation of myTask07 */
  osThreadStaticDef(myTask07, StartTask07, osPriorityIdle, 0, 70, myTask07Buffer, &myTask07ControlBlock);
  myTask07Handle = osThreadCreate(osThread(myTask07), NULL);

  /* definition and creation of myTask08 */
  osThreadStaticDef(myTask08, StartTask08, osPriorityIdle, 0, 70, myTask08Buffer, &myTask08ControlBlock);
  myTask08Handle = osThreadCreate(osThread(myTask08), NULL);

// disable the last 2 tasks or it hangs - not enough memory, I guess
  /* definition and creation of myTask09 */
  osThreadStaticDef(myTask09, StartTask09, osPriorityIdle, 0, 70, myTask09Buffer, &myTask09ControlBlock);
  myTask09Handle = osThreadCreate(osThread(myTask09), NULL);

  /* definition and creation of myTask10 */
  osThreadStaticDef(myTask10, StartTask10, osPriorityIdle, 0, 70, myTask10Buffer, &myTask10ControlBlock);
  myTask10Handle = osThreadCreate(osThread(myTask10), NULL);

  /* definition and creation of myTask11 */
  osThreadStaticDef(myTask11, StartTask11, osPriorityIdle, 0, 70, myTask11Buffer, &myTask11ControlBlock);
  myTask11Handle = osThreadCreate(osThread(myTask11), NULL);
#endif
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}
#endif
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
  /* USER CODE BEGIN StartDefaultTask */
  	menu_ptr = 0;
    password_valid = 0;
    password_ptr = 0;
	password_retries = 0;

	key_mode = PASSWORD;
//	key_mode = NORMAL;
	DWT_Delay_Init();
//	osTimerStart(myTimer01Handle,1000);

#if 0
	memset(correct_password,0,sizeof(correct_password));
	strcpy(correct_password,"2354795\0");
	memset(password,0,PASSWORD_SIZE);
	osTimerStart(myTimer01Handle,1000);
//	avr_buffer[0] = PASSWORD_MODE;
#endif
  /* Infinite loop */

	xbyte = 0x21;
	for(i = 0;i < 20;i++)
	{
  		if(menu_ptr == 0)
		{
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
//			HAL_UART_Transmit(&huart1, &xbyte, 1, 100);
//			if(++xbyte > 0x7e)
//				xbyte = 0x21;
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

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartBasicCmdTask(void const * argument)
{
	UCHAR key;
	UCHAR cmd;
	unsigned long recval;
	unsigned long sendval; 
		
	lights_on = 0;
	brights_on = 0;
	fan_on = 0;
	blower_on = 0;
	engine_on = 0;

	sendval = 8;
//	xQueueSend(FlashQueueHandle, &sendval, 0);	

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
					sendval = (unsigned long)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
					HAL_GPIO_WritePin(LD3_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
				}
			break;
			case KP_2:
				cmd = SHUTDOWN;
				sendval = (unsigned long)cmd;
				xQueueSend(Send7200QueueHandle, &sendval, 0);
				cmd = OFF_FAN;
				sendval = (unsigned long)cmd;
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
						sendval = (unsigned long)cmd;
						xQueueSend(Send7200QueueHandle, &sendval, 0);
						brights_on = 1;
					}
					else
					{
						cmd = OFF_BRIGHTS;
						sendval = (unsigned long)cmd;
						xQueueSend(Send7200QueueHandle, &sendval, 0);
						brights_on = 0;
					}
				}
			break;
			case KP_4:
				if(fan_on == 0)
				{
					cmd = ON_FAN;
					sendval = (unsigned long)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
					fan_on = 1;
					HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
				}
				else
				{
					cmd = OFF_FAN;
					sendval = (unsigned long)cmd;
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
					sendval = (unsigned long)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
					break;
					case 1:
					cmd = BLOWER1;
					sendval = (unsigned long)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
					break;
					case 2:
					cmd = BLOWER2;
					sendval = (unsigned long)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
					break;
					case 3:
					cmd = BLOWER3;
					sendval = (unsigned long)cmd;
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
					sendval = (unsigned long)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
				}else
				{
					running_lights_on = 0;
					cmd = OFF_RUNNING_LIGHTS;
					sendval = (unsigned long)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
				}
			break;
			case KP_7:
				if(lights_on == 0)
				{
					cmd = ON_LIGHTS;
					sendval = (unsigned long)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
					lights_on = 1;
					sendval = 4;
//					xQueueSend(FlashQueueHandle, &sendval, 0);
				}
				else
				{
					cmd = OFF_LIGHTS;
					lights_on = 0;
					sendval = (unsigned long)cmd;
					xQueueSend(Send7200QueueHandle, &sendval, 0);
					sendval = 2;
//					xQueueSend(FlashQueueHandle, &sendval, 0);
					if(brights_on == 1)
					{
						cmd = OFF_BRIGHTS;
						sendval = (unsigned long)cmd;
						xQueueSend(Send7200QueueHandle, &sendval, 0);
						brights_on = 0;
					}
				}
			break;
			case KP_8:
/*				opps - this won't work - not part of basic commands
				cmd = TEST_LEFT_BLINKER;	-- can only be sent via TCP
				sendval = (unsigned long)cmd;
				xQueueSend(Send7200QueueHandle, &sendval, 0);
*/
				sendval = 8;
//				xQueueSend(FlashQueueHandle, &sendval, 0);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

			break;
			case KP_9:
/*
				cmd = TEST_RIGHT_BLINKER;
				sendval = (unsigned long)cmd;
				xQueueSend(Send7200QueueHandle, &sendval, 0);
*/
				sendval = 4;
//				xQueueSend(FlashQueueHandle, &sendval, 0);
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);

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
* @brief Function implementing the myTask03 thread.
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
* @brief Function implementing the myTask04 thread.
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
* @brief Function implementing the myTask05 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask05 */
void StartTask7200(void const * argument)
{
	unsigned long ulReceivedValue;
	UCHAR cmd;

	for(;;)
	{
		xQueueReceive(Send7200QueueHandle, &ulReceivedValue, portMAX_DELAY);
		cmd = (UCHAR)ulReceivedValue;
		HAL_UART_Transmit(&huart1, &cmd, 1, 100);
		vTaskDelay(10);
	}
}

/* USER CODE BEGIN Header_StartTask06 */
/**
* @brief Function implementing the myTask06 thread.
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
				HAL_UART_Transmit(&huart2, ucbuff, 1, 100);
				break;
			default:
				break;
		}
		end_byte = 0xFE;
		HAL_UART_Transmit(&huart2, &end_byte, 1, 100);
	}
}


#if 0
/* USER CODE BEGIN Header_StartTask07 */
/**
* @brief Function implementing the myTask07 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask07 */
void StartTask07(void const * argument)
{
  /* USER CODE BEGIN StartTask07 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask07 */
}

/* USER CODE BEGIN Header_StartTask08 */
/**
* @brief Function implementing the myTask08 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask08 */
void StartTask08(void const * argument)
{
  /* USER CODE BEGIN StartTask08 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask08 */
}

/* USER CODE BEGIN Header_StartTask09 */
/**
* @brief Function implementing the myTask09 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask09 */
void StartTask09(void const * argument)
{
  /* USER CODE BEGIN StartTask09 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask09 */
}

/* USER CODE BEGIN Header_StartTask10 */
/**
* @brief Function implementing the myTask10 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask10 */
void StartTask10(void const * argument)
{
  /* USER CODE BEGIN StartTask10 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask10 */
}

/* USER CODE BEGIN Header_StartTask11 */
/**
* @brief Function implementing the myTask11 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask11 */
void StartTask11(void const * argument)
{
	UCHAR rec_char;
	UCHAR row, col;
	UCHAR str;
	UCHAR xbyte;
	int i, j;
	uint64_t avr_buffer[5];
	UCHAR ucbuff[8];

//	memset(avr_buffer,0,20);

	vTaskDelay(10);

	clr_scr();
	vTaskDelay(10);

	ucbuff[0] = CHAR_CMD;
	xbyte = 0x21;
	ucbuff[2] = 0xFE;
	goto_scr(0,0);

	for(;;)
	{
	
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

		vTaskDelay(1000);
		clr_scr();
	}
#if 0
	row = 1;
	col = 0;
	str = 0;

	ucbuff[0] = EEPROM_STR;
	ucbuff[4] = 15;
	ucbuff[5] = 0xFE;

	j = 0;
	for(;;)
	{
		ucbuff[1] = row;
		ucbuff[2] = col;
		ucbuff[3] = str;
		avr_buffer[0] = pack64(ucbuff);
		xQueueSend(SendAVRQueueHandle,avr_buffer,0);
		vTaskDelay(500);

		if(++row > 15)
		{
			row = 1;
			col = 20;
		}

		if(++str > 31)
		{
//			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
			vTaskDelay(1500);
			str = 0;
			col = 0;
			row = 1;

			clr_scr();
			
			goto_scr(0,0);
/*
			if(++j > 10)
			{
				j = 0;
				avr_buffer[0] = CHAR_CMD;
				xbyte = 0x21;
				avr_buffer[2] = 0xFE;
				goto_scr(0,0);
				
				for(row = 0;row < ROWS;row++)
					for(col = 0;col < COLUMN;col++)
					{
						avr_buffer[1] = xbyte;
						xQueueSend(SendAVRQueueHandle,avr_buffer,0);
						vTaskDelay(5);
						if(++xbyte > 0x7e)
							xbyte = 0x21;
					}
				avr_buffer[0] = EEPROM_STR;
				avr_buffer[4] = 15;
				avr_buffer[5] = 0xFE;

			}
*/
		}
	}
#endif
}
#endif

/* Callback01 function */
void Callback01(void const * argument)
{
#if 0
	if(menu_ptr == 0)
	{
//		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
		menu_ptr = 1;
	}else
	{
//		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
//		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
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
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
