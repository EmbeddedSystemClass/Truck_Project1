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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "dwt_stm32_delay.h"
#include "../mytypes.h"
#include "ds1620.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//#define mainQUEUE_SEND_FREQUENCY_MS			( 3000 / portTICK_PERIOD_MS )
static int lights_on;
static int brights_on;
static int blower_on;
static int fan_on;
static int engine_on;
static int running_lights_on;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
enum key_types
{
 	KP_1 = 0xE0, // '1'		- E0
	KP_2, // '2'		- E1
	KP_3, // '3'		- E2
	KP_4, // '4'		- E3
	KP_5, // '5'		- E4
	KP_6, // '6'		- E5
	KP_7, // '7'		- E6
	KP_8, // '8'		- E7
	KP_9, // '9'		- E8
	KP_A, // 'A'		- E9
	KP_B, // 'B'		- EA
	KP_C, // 'C'		- EB
	KP_D, // 'D'		- EC
	KP_POUND,	// '#'	- ED
	KP_AST, // '*'		- EE
	KP_0 	// '0'		- EF
} KEY_TYPES;
/*	for testing
	KP_0 = 0x30,
	KP_1, // '1'		- E0
	KP_2, // '2'		- E1
	KP_3, // '3'		- E2
	KP_4, // '4'		- E3
	KP_5, // '5'		- E4
	KP_6, // '6'		- E5
	KP_7, // '7'		- E6
	KP_8, // '8'		- E7
	KP_9, // '9'		- E8
	KP_A = 'A',
	KP_B, // 'B'
	KP_C, // 'C'
	KP_D, // 'D'
	KP_POUND = '#',
	KP_AST = '*'
} KEY_TYPES;
*/
#define col0 col0_Pin
#define col1 col1_Pin
#define col2 col2_Pin
#define col3 col3_Pin

#define row0 row0_Pin
#define row1 row1_Pin
#define row2 row2_Pin
#define row3 row3_Pin

#define NUM_ROWS 4
#define NUM_COLS 4

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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
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

/* static void drive_row_low(void)
{
	HAL_GPIO_WritePin(GPIOB, row0, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, row1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, row2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, row3, GPIO_PIN_RESET);
}

 */static void drive_row_high(void)
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

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

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
//			HAL_UART_Transmit(&huart2, &key_pressed, 1, 100);
//				vTaskDelay(1);
		}
		state = HAL_GPIO_ReadPin(GPIOC,col1);
		if(state == GPIO_PIN_RESET)
		{
			key_pressed = (au8_keyTable[u8_row][1]);
			return key_pressed;
//			HAL_UART_Transmit(&huart2, &key_pressed, 1, 100);
//				vTaskDelay(1);
		}
		state = HAL_GPIO_ReadPin(GPIOC,col2);
		if(state == GPIO_PIN_RESET)
		{
			key_pressed = (au8_keyTable[u8_row][2]);
			return key_pressed;
//			HAL_UART_Transmit(&huart2, &key_pressed, 1, 100);
//				vTaskDelay(1);
		}
		state = HAL_GPIO_ReadPin(GPIOC,col3);
		if(state == GPIO_PIN_RESET)
		{
			key_pressed = (au8_keyTable[u8_row][3]);
			return key_pressed;
//			HAL_UART_Transmit(&huart2, &key_pressed, 1, 100);
		}
//		vTaskDelay(1);
	}
	return 0;
}


/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId Task02Handle;
osThreadId Task03Handle;
osThreadId Task04Handle;
osThreadId KeyPressedHandle;
osThreadId FlashTaskHandle;
osThreadId DS1620TaskHandle;
osThreadId TestTaskHandle;
osMessageQId keypressedQueueHandle;
osMessageQId Send7200QueueHandle;
osMessageQId FlashQueueHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartTask02(void const * argument);
void StartTask03(void const * argument);
void StartTask04(void const * argument);
void StartKeyPressed(void const * argument);
void StartTask06(void const * argument);
void StartDS1620Task(void const * argument);
void StartTestTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

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

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of keypressedQueue */
  osMessageQDef(keypressedQueue, 16, uint16_t);
  keypressedQueueHandle = osMessageCreate(osMessageQ(keypressedQueue), NULL);

  /* definition and creation of Send7200Queue */
  osMessageQDef(Send7200Queue, 16, uint16_t);
  Send7200QueueHandle = osMessageCreate(osMessageQ(Send7200Queue), NULL);

  /* definition and creation of FlashQueue */
  osMessageQDef(FlashQueue, 16, uint16_t);
  FlashQueueHandle = osMessageCreate(osMessageQ(FlashQueue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of Task02 */
  osThreadDef(Task02, StartTask02, osPriorityIdle, 0, 128);
  Task02Handle = osThreadCreate(osThread(Task02), NULL);

  /* definition and creation of Task03 */
  osThreadDef(Task03, StartTask03, osPriorityIdle, 0, 128);
  Task03Handle = osThreadCreate(osThread(Task03), NULL);

  /* definition and creation of Task04 */
  osThreadDef(Task04, StartTask04, osPriorityIdle, 0, 128);
  Task04Handle = osThreadCreate(osThread(Task04), NULL);

  /* definition and creation of KeyPressed */
  osThreadDef(KeyPressed, StartKeyPressed, osPriorityIdle, 0, 128);
  KeyPressedHandle = osThreadCreate(osThread(KeyPressed), NULL);

  /* definition and creation of FlashTask */
//  osThreadDef(FlashTask, StartTask06, osPriorityIdle, 0, 128);
//  FlashTaskHandle = osThreadCreate(osThread(FlashTask), NULL);

//  osThreadDef(DS1620Task, StartDS1620Task, osPriorityIdle, 0, 128);
//  DS1620TaskHandle = osThreadCreate(osThread(DS1620Task), NULL);

  osThreadDef(TestTask, StartTestTask, osPriorityIdle, 0, 128);
  TestTaskHandle = osThreadCreate(osThread(TestTask), NULL);

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

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
//		HAL_UART_Transmit(&huart1,buff,93,100);
		vTaskDelay(2000);

  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the Task02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
	unsigned long ulReceivedValue;
	UCHAR cmd;
  /* Infinite loop */
	for(;;)
	{
		xQueueReceive(Send7200QueueHandle, &ulReceivedValue, portMAX_DELAY);
		cmd = (UCHAR)ulReceivedValue;
		HAL_UART_Transmit(&huart1, &cmd, 1, 100);
		vTaskDelay(1);
	}
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the Task03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void const * argument)
{
  /* USER CODE BEGIN StartTask03 */
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
	xQueueSend(FlashQueueHandle, &sendval, 0);	

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
		HAL_UART_Transmit(&huart2, &key, 1, 100);
		vTaskDelay(10);
	}

  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the Task04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartTask04(void const * argument)
{
  /* USER CODE BEGIN StartTask04 */
	static UCHAR key;
	unsigned long sendval;
  
  /* Infinite loop */
	e_isrState = STATE_WAIT_FOR_PRESS;  

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
//					HAL_UART_Transmit(&huart2, &key, 1, 100);
					drive_row_high();
					sendval = (unsigned long)key;
					xQueueSend(keypressedQueueHandle, &sendval, 0);
					key = 0;
				}
				break;
			default:
//				HAL_UART_Transmit(&huart2, &e_isrState, 1, 100);
				e_isrState = STATE_WAIT_FOR_PRESS;
				break;
		}
//		temp = e_isrState + 0x30;
//		HAL_UART_Transmit(&huart2, &temp, 1, 100);
		vTaskDelay(1);
	}
  /* USER CODE END StartTask04 */
}

/* USER CODE BEGIN Header_StartKeyPressed */
/**
* @brief Function implementing the KeyPressed thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartKeyPressed */
void StartKeyPressed(void const * argument)
{
  /* USER CODE BEGIN StartKeyPressed */
//	unsigned long ulReceivedValue;

	/* Infinite loop */
	for(;;)
	{
		osDelay(1);
	}
  /* USER CODE END StartKeyPressed */
}

/* USER CODE BEGIN Header_StartTask06 */
/**
* @brief Function implementing the FlashTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask06 */
void StartTask06(void const * argument)
{
  /* USER CODE BEGIN StartTask06 */
	int i;
	unsigned long recval;
	UCHAR cmd;
  /* Infinite loop */
	for(;;)
	{
		xQueueReceive(FlashQueueHandle, &recval, portMAX_DELAY);
		cmd = (UCHAR)recval;

		for(i = 0;i < cmd;i++)
		{
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
			vTaskDelay(50);
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
			vTaskDelay(50);
		}
		// restore lights to original state
		if(engine_on == 0)
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);

		if(fan_on == 0)
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		else
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
	}
  /* USER CODE END StartTask06 */
}

/* USER CODE END Header_StartTask02 */
void StartDS1620Task(void const * argument)
{
  /* USER CODE BEGIN StartTask02 */
	uint16_t utemp;
	UCHAR temp;
	int raw_data;

	DWT_Delay_Init();
//	gpio_set_direction(OUTPUT);
  /* Infinite loop */
	vTaskDelay(3000);
	
	for(;;)
	{
		vTaskDelay(1000);
		writeByteTo1620(DS1620_CMD_STARTCONV);
		raw_data = readTempFrom1620();
		writeByteTo1620(DS1620_CMD_STOPCONV);
		temp = (UCHAR)raw_data;
//		printHexByte(temp);
		raw_data >>= 8;
		temp = (UCHAR)raw_data;
//		printHexByte(temp);
	}
  /* USER CODE END StartTask02 */
}

/* USER CODE END Header_StartTestTask */
void StartTestTask(void const * argument)
{
	UCHAR key[6];
	UCHAR rec_char;
	UCHAR row, col;
	UCHAR str;
	UCHAR xbyte;
	/* USER CODE BEGIN StartTask02 */

	key[0] = CHAR_CMD;
	xbyte = 0x21;
	key[2] = 0xFE;
	for(row = 0;row < ROWS;row++)
		for(col = 0;col < COLUMN;col++)
		{
			key[1] = xbyte;
			HAL_UART_Transmit(&huart2, key, 3, 100);
			vTaskDelay(2);
			if(++xbyte > 0x7e)
				xbyte = 0x21;
		}

	row = col = 0;
	str = 1;
	key[0] = EEPROM_STR;
	key[1] = row;
	key[2] = col;
	key[3] = str + 2;
	key[4] = 4;
	key[5] = 0xFE;
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
	
	
	for(;;)
	{
		vTaskDelay(200);
		HAL_UART_Transmit(&huart2, key, 6, 100);
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);

		do
		{
			HAL_UART_Receive(&huart2, &rec_char, 1, 100);
			vTaskDelay(2);
		}while(rec_char != 0xFD);

		if(++row > 15)
		{
			row = 0;
			col = 20;
		}

		if(++str > 30)
		{
			str = 1;
			col = row = 0;
			key[0] = LCD_CLRSCR;
			key[1] = 0xFE;
			HAL_UART_Transmit(&huart2, key, 2, 100);
			row = col = 0;
		}
		vTaskDelay(200);

		key[0] = EEPROM_STR;
		key[1] = row;
		key[2] = col;
		key[3] = str;
		key[4] = 3;
		key[5] = 0xFE;

		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
	}
	/* USER CODE END StartTestTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
 	UCHAR crow;

 	crow = (UCHAR)GPIO_Pin;
	crow += 0x30;
	HAL_UART_Transmit(&huart2, &crow, 1, 100);

}	
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
