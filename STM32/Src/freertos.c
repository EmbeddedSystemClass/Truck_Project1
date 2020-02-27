/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "dwt_stm32_delay.h"
#include "../mytypes.h"
#include "ds1620.h"	
#include "freertos_defs.h"
#include "usart.h"
#include "screen.h"
#include "string.h"
#include "stdlib.h"
#include "gpio.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define SEND_FPGA_DELAY() vTaskDelay(5)
// WARNING! - if this gets pulled up on STM32CubeMX it will reset the queues
// to 16 of uint16_t instead of  5 of uint64_t which has to be manually setOneRowLow
// I just forgot to change it when I redid the whole thing under CubeMX
// however, the keypressedHandle is still 16 of uint16_t tho...
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

uint64_t pack64(UCHAR *buff);
//uint32_t pack32(UCHAR *buff);

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
// variables used by the DS1620 conversion in timer Callback01
uint64_t global_avr_buffer[5];
UCHAR global_ucbuff[8];
UCHAR global_cmd;
static int raw_data;	// engine temp
static int raw_data2;	// indoor temp
static int temp_raw;
static int test_rise_temp;
static int too_high_flag;
static int cooling_fan_override;
static int blower_override;
static uint16_t engine_temp_limit = 190;
static uint16_t fan_on_temp = 175;	// 185F
static uint16_t fan_off_temp = 150;
static uint16_t blower1_temp = 65;
static uint16_t blower2_temp = 55;
static uint16_t blower3_temp = 45;
static uint16_t blower_en_temp = 70;
static uint16_t batt_box_temp = 75;
static int blower_on_countdown = 5;
static int send_adc = 0;
//static uint16_t lights_on_delay = 5;	// default is 3 seconds
//static UCHAR key_mode1, key_mode2;
static int server_up = 0;
//static UCHAR buff[SERIAL_BUFF_SIZE+1];
static int timer_toggle;
static int blower_on;
static int fan_on;
static int engine_on;
//static int system_up;
static int run_time;
static int rtdata_update_rate;

static KEY_MODE key_mode;
/*	(from freertos_defs.h)
typedef enum
{
	NORMAL,
	PASSWORD,
	NUM_ENTRY,
}KEY_MODE;
*/
static char password[PASSWORD_SIZE];
static char correct_password[PASSWORD_SIZE];
static int password_ptr = 0;
static int password_retries = 3;
static int password_countdown = 0;
static int init_password_retries = 3;
static int init_password_countdown = 10;

//static char num_entry_num[SIZE_NUM];
//static int task7on;

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
osThreadId myTask04Handle;
osThreadId myTask05Handle;
osThreadId myTask06Handle;
osThreadId myTask08Handle;
osThreadId myTask09Handle;
osThreadId myTask10Handle;
osMessageQId keypressedHandle;
osMessageQId Send7200Handle;
osMessageQId SendMonitorHandle;
osMessageQId SendFPGAHandle;
osTimerId myTimer01Handle;

static void check_password(int num)
{
	UCHAR cmd;
	uint64_t avr_buffer[5];
	UCHAR ucbuff[8];
	int i;
/*
	cmd = PASSWORD_BAD;
	ucbuff[0] = cmd;
	ucbuff[1] = (UCHAR)num;
	ucbuff[2] = (UCHAR)password_ptr;
	avr_buffer[0] = pack64(ucbuff);
	xQueueSend(Send7200Handle, avr_buffer, 0);
	vTaskDelay(100);
*/
	itoa(num, &password[password_ptr],10);
	if(password[password_ptr] != correct_password[password_ptr])
	{
/*
		cmd = PASSWORD_BAD;
		ucbuff[0] = cmd;
		ucbuff[1] = (UCHAR)num;
		ucbuff[2] = (UCHAR)password_retries;
		avr_buffer[0] = pack64(ucbuff);
		xQueueSend(Send7200Handle, avr_buffer, 0);
*/
		password_ptr = 0;
		password_retries--;
		password_countdown = init_password_countdown;
		memset(password,0,PASSWORD_SIZE);

		if(password_retries < 1)
		{
			// tell TS-7200 to turn off engine
			password_countdown = 0;
			password_retries = init_password_retries;
			ucbuff[0] = SPECIAL_TONE_ON;
			avr_buffer[0] = pack64(ucbuff);
			xQueueSend(SendFPGAHandle,avr_buffer,0);
			vTaskDelay(1000);
			global_ucbuff[0] = DTMF_TONE_OFF;
			global_avr_buffer[0] = pack64(ucbuff);
			xQueueSend(SendFPGAHandle, avr_buffer,0);
			vTaskDelay(20);
			cmd = SHUTDOWN;
			ucbuff[0] = cmd;
			avr_buffer[0] = pack64(ucbuff);
			xQueueSend(Send7200Handle, avr_buffer, 0);
		}
//	}else if(password_ptr == password_len)
	}else if(password_ptr >= 3 && strcmp(password,correct_password) == 0)
	{
		password_countdown = password_ptr = 0;
		password_retries = init_password_retries;

		key_mode = NORMAL;
//		cmd = PASSWORD_OK;
		ucbuff[0] = cmd;
		avr_buffer[0] = pack64(ucbuff);
		xQueueSend(Send7200Handle, avr_buffer, 0);

		for(i = 0;i < 5;i++)
		{
			global_ucbuff[0] = DTMF_TONE_ON;
			global_ucbuff[1] = i;
			global_avr_buffer[0] = pack64(global_ucbuff);
			xQueueSend(SendFPGAHandle,global_avr_buffer,0);
			vTaskDelay(20);
			global_ucbuff[0] = DTMF_TONE_OFF;
			global_avr_buffer[0] = pack64(global_ucbuff);
			xQueueSend(SendFPGAHandle,global_avr_buffer,0);
			vTaskDelay(20);

		}
		password_countdown = 0;
		key_mode = NORMAL;
	}else
		password_ptr++;
}

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartMonitorTask(void const * argument);
void StartTask7200(void const * argument);
void StartRecv7200(void const * argument);
void StartSendFPGA(void const * argument);
void StartRecvFPGA(void const * argument);
void StartKeyStateTask(void const * argument);
void Callback01(void const * argument);

//#endif
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

  /* Create the timer(s) */
  /* definition and creation of myTimer01 */
  osTimerDef(myTimer01, Callback01);
  myTimer01Handle = osTimerCreate(osTimer(myTimer01), osTimerPeriodic, NULL);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of myTask04 */
//  osThreadDef(myTask04, StartMonitorTask, osPriorityIdle, 0, 128);
//  myTask04Handle = osThreadCreate(osThread(myTask04), NULL);

  /* definition and creation of myTask05 */
  osThreadDef(myTask05, StartTask7200, osPriorityIdle, 0, 128);
  myTask05Handle = osThreadCreate(osThread(myTask05), NULL);

  /* definition and creation of myTask06 */
  osThreadDef(myTask06, StartRecv7200, osPriorityIdle, 0, 128);
  myTask06Handle = osThreadCreate(osThread(myTask06), NULL);

  /* definition and creation of myTask08 */
  osThreadDef(myTask08, StartSendFPGA, osPriorityIdle, 0, 128);
  myTask08Handle = osThreadCreate(osThread(myTask08), NULL);

  /* definition and creation of myTask09 */
  osThreadDef(myTask09, StartRecvFPGA, osPriorityIdle, 0, 128);
  myTask09Handle = osThreadCreate(osThread(myTask09), NULL);

  /* definition and creation of myTask10 */
  osThreadDef(myTask10, StartKeyStateTask, osPriorityIdle, 0, 128);
  myTask10Handle = osThreadCreate(osThread(myTask10), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the queue(s) */
  /* definition and creation of keypressed */
  osMessageQDef(keypressed, 16, uint16_t);
  keypressedHandle = osMessageCreate(osMessageQ(keypressed), NULL);

  /* definition and creation of Send7200 */
  osMessageQDef(Send7200, 10, uint64_t);
  Send7200Handle = osMessageCreate(osMessageQ(Send7200), NULL);

  /* definition and creation of SendTS7800 */
  osMessageQDef(SendTS7800, 10, uint64_t);
  SendMonitorHandle = osMessageCreate(osMessageQ(SendTS7800), NULL);

  /* definition and creation of SendFPGA */
  osMessageQDef(SendFPGA, 10, uint64_t);
  SendFPGAHandle = osMessageCreate(osMessageQ(SendFPGA), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* note: Send7200, SendTS7800 & SendFPGA must be reset from uint16_t to uint64_t */
  /* after pulling up in MXCube */
  /* USER CODE END RTOS_QUEUES */
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

  /* USER CODE BEGIN StartDefaultTask */
	uint64_t avr_buffer[5];
	UCHAR ucbuff[8];
	UCHAR key;
	UCHAR cmd;
	uint16_t recval;
//	int row, col;

	fan_on = 0;
	blower_on = 0;
	engine_on = 0;
  	timer_toggle = 0;
	run_time = 0;
//	key_mode = PASSWORD;
	key_mode = NORMAL;
	temp_raw = raw_data = 4;
	raw_data2 = 4;
	cooling_fan_override = 0;
	blower_override = 0;

	memset(correct_password,0,sizeof(correct_password));
	strcpy(correct_password,"4567\0");
	memset(password,0,PASSWORD_SIZE);
	osTimerStart(myTimer01Handle,1000);
//	DWT_Delay_Init();		// for using DWT_Delay_us(microseconds) delay (but why?)

	vTaskDelay(1000);

	init_DS1620();
	vTaskDelay(10);
	initDS1620();

	init_DS16202();
	vTaskDelay(10);
	initDS16202();

  /* Infinite loop */
 
  for(;;)
  {
		xQueueReceive(keypressedHandle, &recval, portMAX_DELAY);
		key = (UCHAR)recval;
		cmd = 0;

		ucbuff[0] = key;
		avr_buffer[0] = pack64(ucbuff);
		xQueueSend(Send7200Handle, avr_buffer, 0);
		switch(key)
		{
			case KP_A:
				ucbuff[0] = NAV_UP;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(Send7200Handle, avr_buffer, 0);
			break;

			case KP_B:
				ucbuff[0] = NAV_DOWN;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(Send7200Handle, avr_buffer, 0);
			break;

			case KP_C:
				ucbuff[0] = NAV_CLICK;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(Send7200Handle, avr_buffer, 0);
			break;

			case KP_D:
				ucbuff[0] = NAV_CLOSE;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(Send7200Handle, avr_buffer, 0);
			break;

			case KP_AST:
				ucbuff[0] = NAV_SIDE;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(Send7200Handle, avr_buffer, 0);
			break;

			case KP_POUND:
/*
				if(key_mode == NORMAL)
				{
					running_lights_on = 1;
					cmd = ON_RUNNING_LIGHTS;
					ucbuff[0] = cmd;
					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(Send7200Handle, avr_buffer, 0);
					vTaskDelay(200);
					cmd = ON_LIGHTS;
					ucbuff[0] = cmd;
					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(Send7200Handle, avr_buffer, 0);
					lights_on = 1;
				}
*/
			break;

			default:
				ucbuff[0] = DTMF_TONE_ON;
				ucbuff[1] = 10;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendFPGAHandle,avr_buffer,0);
				vTaskDelay(20);
				ucbuff[0] = DTMF_TONE_ON;
				ucbuff[1] = 9;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendFPGAHandle,avr_buffer,0);
				vTaskDelay(20);
				ucbuff[0] = DTMF_TONE_OFF;
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(SendFPGAHandle,avr_buffer,0);
			break;
		}
		vTaskDelay(10);
	}
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartMonitorTask */
/**
* @brief Function implementing the myTask04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMonitorTask */
void StartMonitorTask(void const * argument)
{
  /* USER CODE BEGIN StartMonitorTask */
	uint64_t avr_buffer[10];
	UCHAR ucbuff[10];
	UCHAR temp;

  /* Infinite loop */

	for(;;)
	{
		xQueueReceive(SendMonitorHandle, avr_buffer, portMAX_DELAY);
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

//			HAL_UART_Transmit(&huart2, &start_byte, 1, 100);
	}
  /* USER CODE END StartMonitorTask */
}

/* USER CODE BEGIN Header_StartTask7200 */
/**
* @brief Function implementing the myTask05 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask7200 */
void StartTask7200(void const * argument)
{
  /* USER CODE BEGIN StartTask7200 */
  	uint64_t avr_buffer[5];
	UCHAR ucbuff[SERIAL_BUFF_SIZE];
	UCHAR marker = 0xFF;

	memset(ucbuff,0,sizeof(ucbuff));
	vTaskDelay(15000);
	/* Infinite loop */
	for(;;)
	{
		xQueueReceive(Send7200Handle, avr_buffer, portMAX_DELAY);

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

//		if(system_up == 1)
		if(server_up == 1)
		{
			HAL_UART_Transmit(&huart1, &marker, 1, 100);
			HAL_UART_Transmit(&huart1, &ucbuff[0], SERIAL_BUFF_SIZE, 100);
		}
		//		HAL_UART_Transmit_IT(&huart1, &marker, 1);
		//		HAL_UART_Transmit_IT(&huart1, &ucbuff[0], SERIAL_BUFF_SIZE);
		/*
		HAL_UART_Receive(&huart1, &Recv7200buff[0], SERIAL_BUFF_SIZE, 1000);


		vTaskDelay(100);
		*/
	}
  /* USER CODE END StartTask7200 */
}

/* USER CODE BEGIN Header_StartRecv7200 */
/**
* @brief Function implementing the myTask06 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartRecv7200 */
void StartRecv7200(void const * argument)
{
  /* USER CODE BEGIN StartRecv7200 */
	UCHAR cmd;
	int i;
//	int j = 99;
//	int k = 0;
	UINT utemp;
	uint64_t avr_buffer[5];

	UCHAR buff[SERIAL_BUFF_SIZE+1];
//  	uint64_t avr_buffer[5];
//	UCHAR ucbuff[SERIAL_BUFF_SIZE];
//	UCHAR marker = 0xFF;
//	UCHAR tone;
	test_rise_temp = 0;
	too_high_flag = 0;
	UCHAR ucbuff[10];

	/* Infinite loop */
	vTaskDelay(1000);
	for(;;)
	{
//		HAL_UART_Receive_IT(&huart1, &cmd, 1);
//		memset(buff,0,SERIAL_BUFF_SIZE+1);
		HAL_UART_Receive_IT(&huart1, &buff[0], SERIAL_BUFF_SIZE+1);
		cmd = buff[0];

		if((cmd >= ENABLE_START && cmd <= WIPER_OFF) || cmd == SERVER_UP || 
				cmd == SERVER_DOWN || cmd == SEND_CONFIG2 || cmd == GET_CONFIG2 || 
				cmd == ADC_GATE || cmd == SET_ADC_RATE || cmd == SET_FPGA_RATE || 
				cmd == SET_RPM_MPH_RATE || cmd == HIGH_REV_LIMIT || cmd == LOW_REV_LIMIT || 
				cmd == SEND_REV_LIMIT_OVERRIDE || cmd == SEND_FP_OVERRIDE)
		{
			switch (cmd)
			{
				case 	ON_FAN:				// 7
					fan_on = 1;
					break;
				case 	OFF_FAN:			// 8
					fan_on = 0;
					break;
				case 	SPECIAL_CMD:
					break;
				case 	START_SEQ:
					engine_on = 1;
					blower_on_countdown = 5;
					break;
				case 	SHUTDOWN:
				case 	ESTOP_SIGNAL:
					engine_on = 0;
					break;

				case SERVER_UP:

					engine_temp_limit = (uint16_t)buff[2];
					engine_temp_limit <<= 8;
					engine_temp_limit |= (uint16_t)buff[1];

					fan_on_temp = (uint16_t)buff[4];
					fan_on_temp <<= 8;
					fan_on_temp |= (uint16_t)buff[3];

 					fan_off_temp = (uint16_t)buff[6];
					fan_off_temp <<= 8;
					fan_off_temp |= (uint16_t)buff[5];

 					blower1_temp = (uint16_t)buff[8];
					blower1_temp <<= 8;
					blower1_temp |= (uint16_t)buff[7];

 					blower2_temp = (uint16_t)buff[10];
					blower2_temp <<= 8;
					blower2_temp |= (uint16_t)buff[9];

 					blower3_temp = (uint16_t)buff[12];
					blower3_temp <<= 8;
					blower3_temp |= (uint16_t)buff[11];

 					blower_en_temp = (uint16_t)buff[14];
					blower_en_temp <<= 8;
					blower_en_temp |= (uint16_t)buff[13];
/*
 					init_password_countdown = (uint16_t)buff[16];
					init_password_countdown <<= 8;
					init_password_countdown |= (uint16_t)buff[15];

 					init_password_retries = (uint16_t)buff[18];
					init_password_retries <<= 8;
					init_password_retries |= (uint16_t)buff[17];
*/
					for(i = 0;i < 20;i++)
					{
						if(timer_toggle == 0)
						{
							HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
							vTaskDelay(50);
							timer_toggle = 1;
						}else
						{
							HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
							vTaskDelay(50);
							timer_toggle = 0;
						}
					}

					HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(LD3_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
/*
					for(i = 0;i < 4;i++)
					{
						ucbuff[0] = DTMF_TONE_ON;
						ucbuff[1] = (UCHAR)i;
						avr_buffer[0] = pack64(ucbuff);
						xQueueSend(SendFPGAHandle,avr_buffer,0);
						vTaskDelay(10);
						ucbuff[0] = DTMF_TONE_OFF;
						avr_buffer[0] = pack64(ucbuff);
						xQueueSend(SendFPGAHandle,avr_buffer,0);
						vTaskDelay(10);
					}
*/
					server_up = 1;
					break;

				case SERVER_DOWN:
/*
					for(i = 4;i > 0;i--)
					{
						ucbuff[0] = DTMF_TONE_ON;
						ucbuff[1] = (UCHAR)i;
						avr_buffer[0] = pack64(ucbuff);
						xQueueSend(SendFPGAHandle,avr_buffer,0);
						vTaskDelay(10);
						ucbuff[0] = DTMF_TONE_OFF;
						avr_buffer[0] = pack64(ucbuff);
						xQueueSend(SendFPGAHandle,avr_buffer,0);
						vTaskDelay(10);
					}
*/
					for(i = 0;i < 30;i++)
					{
						if(timer_toggle == 0)
						{
							HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
							vTaskDelay(30);
							timer_toggle = 1;
						}else
						{
							HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
							HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
							vTaskDelay(30);
							timer_toggle = 0;
						}
					}
					vTaskDelay(10);
					server_up = 0;
					break;

				case SEND_CONFIG2:
					ucbuff[0] = SET_RPM_MPH_UPDATE_RATE;
					ucbuff[1] = buff[1];
					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(SendFPGAHandle,avr_buffer,0);
					vTaskDelay(100);

					ucbuff[0] = SET_FPGA_SEND_UPDATE_RATE;
					ucbuff[1] = buff[2];
					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(SendFPGAHandle,avr_buffer,0);
					vTaskDelay(100);

					ucbuff[0] = SET_HIGH_REV_LIMIT;
					ucbuff[1] = buff[3];
					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(SendFPGAHandle,avr_buffer,0);
					vTaskDelay(100);

					ucbuff[0] = SET_LOW_REV_LIMIT;
					ucbuff[1] = buff[4];
					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(SendFPGAHandle,avr_buffer,0);
					vTaskDelay(100);

					fan_on_temp = (uint16_t)buff[6];
					fan_on_temp <<= 8;
					fan_on_temp |= (uint16_t)buff[5];

 					fan_off_temp = (uint16_t)buff[8];
					fan_off_temp <<= 8;
					fan_off_temp |= (uint16_t)buff[7];

 					blower_en_temp = (uint16_t)buff[10];
					blower_en_temp <<= 8;
					blower_en_temp |= (uint16_t)buff[9];

 					blower1_temp = (uint16_t)buff[12];
					blower1_temp <<= 8;
					blower1_temp |= (uint16_t)buff[11];

 					blower2_temp = (uint16_t)buff[14];
					blower2_temp <<= 8;
					blower2_temp |= (uint16_t)buff[13];

 					blower3_temp = (uint16_t)buff[16];
					blower3_temp <<= 8;
					blower3_temp |= (uint16_t)buff[15];

					engine_temp_limit = (uint16_t)buff[18];
					engine_temp_limit <<= 8;
					engine_temp_limit |= (uint16_t)buff[17];

					batt_box_temp = (uint16_t)buff[20];
					batt_box_temp <<= 8;
					batt_box_temp |= (uint16_t)buff[19];
/*
					init_password_countdown = (uint16_t)buff[18];
					init_password_countdown <<= 8;
					init_password_countdown |= (uint16_t)buff[17];

					init_password_retries = (uint16_t)buff[20];
					init_password_retries <<= 8;
					init_password_retries |= (uint16_t)buff[19];

					memcpy(&correct_password[0], &buff[21], 4);
					correct_password[4] = 0;
*/
					break;

				case GET_CONFIG2:
					// this is just to verify we are sending
					// the right data
					// serial send buffer can only send 8
					// bytes at a time so just send
					// 3 temps (plus cmd)
					// we can use the same cmd because it is
					// sent from the tcp switch/case and then
					// the same msg gets recv'd in the serial
					// receive task of the linux server
					ucbuff[0] = GET_CONFIG2;
					utemp = fan_on_temp;
					ucbuff[1] = (UCHAR)utemp;
					utemp >>= 8;
					ucbuff[2] = (UCHAR)utemp;

					utemp = fan_off_temp;
					ucbuff[3] = (UCHAR)utemp;
					utemp >>= 8;
					ucbuff[4] = (UCHAR)utemp;

					utemp = engine_temp_limit;
					ucbuff[5] = (UCHAR)utemp;
					utemp >>= 8;
					ucbuff[6] = (UCHAR)utemp;

					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(Send7200Handle, avr_buffer, 0);
					vTaskDelay(100);
					break;

				case ADC_GATE:
					ucbuff[0] = ADC_CTL;
					ucbuff[1] = buff[1];
					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(SendFPGAHandle,avr_buffer,0);
					if(buff[1] > 0)
						send_adc = 1;
					else send_adc = 0;
					break;

				case SET_ADC_RATE:
					ucbuff[0] = SET_ADC_RATE2;
					ucbuff[1] = buff[1];
					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(SendFPGAHandle,avr_buffer,0);
					break;

				case SET_FPGA_RATE:
					ucbuff[0] = SET_FPGA_SEND_UPDATE_RATE;
					ucbuff[1] = buff[1];
					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(SendFPGAHandle,avr_buffer,0);
					break;

				case SET_RPM_MPH_RATE:
					ucbuff[0] = SET_RPM_MPH_UPDATE_RATE;
					ucbuff[1] = buff[1];
					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(SendFPGAHandle,avr_buffer,0);
					break;

				case HIGH_REV_LIMIT:
					ucbuff[0] = SET_HIGH_REV_LIMIT;
					ucbuff[1] = buff[1];
					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(SendFPGAHandle,avr_buffer,0);
					break;
				
				case LOW_REV_LIMIT:
					ucbuff[0] = SET_LOW_REV_LIMIT;
					ucbuff[1] = buff[1];
					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(SendFPGAHandle,avr_buffer,0);
					break;

				case SEND_REV_LIMIT_OVERRIDE:
					ucbuff[0] = REV_LIMIT_OVERRIDE;
					ucbuff[1] = buff[1];
					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(SendFPGAHandle,avr_buffer,0);
					break;

				case SEND_FP_OVERRIDE:
					ucbuff[0] = FUEL_PUMP_OVERRIDE;
					ucbuff[1] = buff[1];
					avr_buffer[0] = pack64(ucbuff);
					xQueueSend(SendFPGAHandle,avr_buffer,0);
					break;

				default:
					break;
			}
			// relay the message onto the monitor task
			// (or later we can put this with each case stmnt)
			ucbuff[0] = cmd;
			avr_buffer[0] = pack64(ucbuff);
			xQueueSend(SendMonitorHandle,avr_buffer,0);
			memset(buff,0,SERIAL_BUFF_SIZE);
		}
		vTaskDelay(50);
	}
  /* USER CODE END StartRecv7200 */
}

/* USER CODE BEGIN Header_StartSendFPGA */
/**
* @brief Function implementing the myTask08 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSendFPGA */
void StartSendFPGA(void const * argument)
{
  /* USER CODE BEGIN StartSendFPGA */
	UCHAR cmd, param1, param2, param3, param4, param5, param6, param7;
	uint64_t rec_val[10];
//	int i;

	/* Infinite loop */
	for(;;)
	{
		xQueueReceive(SendFPGAHandle, rec_val, portMAX_DELAY);

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
	}

  /* USER CODE END StartSendFPGA */
}

/* USER CODE BEGIN Header_StartRecvFPGA */
/**
* @brief Function implementing the myTask09 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartRecvFPGA */
void StartRecvFPGA(void const * argument)
{
  /* USER CODE BEGIN StartRecvFPGA */
//	int i = 0;
	/* Infinite loop */
	UCHAR xbyte = 0;
	GPIO_PinState state;
	uint64_t avr_buffer[5];
	UCHAR ucbuff[8];
	UCHAR buff[25];
	int frame_ptr;

//	UINT rpm, mph;
//	rpm = 500;
//	mph = 1;

	rtdata_update_rate = 0;

	xbyte = 0x21;

	vTaskDelay(2000);
	HAL_GPIO_WritePin(GPIOA, PP_ACK_Pin, GPIO_PIN_SET);
	vTaskDelay(10);
	HAL_GPIO_WritePin(GPIOA, PP_ACK_Pin, GPIO_PIN_RESET);
	vTaskDelay(1);

	frame_ptr = 0;

  /* Infinite loop */

	for(;;)
	{

		if(server_up > 0)
		{

		do {
			state = HAL_GPIO_ReadPin(GPIOA,PP_CS_Pin);
		}while(state != GPIO_PIN_SET);

		state = HAL_GPIO_ReadPin(GPIOB,PP0_Pin);
		if(state == GPIO_PIN_SET)
			xbyte |= 1;
		state = HAL_GPIO_ReadPin(GPIOB,PP1_Pin);
		if(state == GPIO_PIN_SET)
			xbyte |= 0x2;
		state = HAL_GPIO_ReadPin(GPIOB,PP2_Pin);
		if(state == GPIO_PIN_SET)
			xbyte |= 0x4;
		state = HAL_GPIO_ReadPin(GPIOB,PP3_Pin);
		if(state == GPIO_PIN_SET)
			xbyte |= 0x8;
		state = HAL_GPIO_ReadPin(GPIOB,PP4_Pin);
		if(state == GPIO_PIN_SET)
			xbyte |= 0x10;
		state = HAL_GPIO_ReadPin(GPIOB,PP5_Pin);
		if(state == GPIO_PIN_SET)
			xbyte |= 0x20;
		state = HAL_GPIO_ReadPin(GPIOB,PP6_Pin);
		if(state == GPIO_PIN_SET)
			xbyte |= 0x40;
		state = HAL_GPIO_ReadPin(GPIOA,PP7_Pin);
		if(state == GPIO_PIN_SET)
			xbyte |= 0x80;

		do {
			state = HAL_GPIO_ReadPin(GPIOA,PP_CS_Pin);
		}while(state != GPIO_PIN_RESET);

		if(xbyte == 0xFE)
		{
			frame_ptr = 0;

#if 0
			if(rpm++ > 5000)		// simulate data for testing
				rpm = 500;
			if(++mph > 100)
				mph = 1;
#endif

			if(engine_on != 0)
			{
				ucbuff[0] = SEND_RT_VALUES1;	// rpm & mph
/*
				ucbuff[1] = (UCHAR)rpm;
				ucbuff[2] = (UCHAR)(rpm << 8);
				ucbuff[3] = (UCHAR)mph;
				ucbuff[4] = (UCHAR)(mph << 8);
*/
				ucbuff[1] = buff[0];		// fpga sends the low byte 1st
				ucbuff[2] = buff[1];
				ucbuff[3] = buff[2];
				ucbuff[4] = buff[3];

				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(Send7200Handle, avr_buffer, 0);
			}
				//vTaskDelay(100);

			if(send_adc == 1)
			{
				ucbuff[0] = SEND_RT_VALUES2;	// the 1st 4 MCP ADC values
				ucbuff[1] = buff[4];
				ucbuff[2] = buff[5];
				ucbuff[3] = buff[6];
				ucbuff[4] = buff[7];

				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(Send7200Handle, avr_buffer, 0);

				ucbuff[0] = SEND_RT_VALUES3;	// the 2nd 4 MCP ADC values
				ucbuff[1] = buff[8];
				ucbuff[2] = buff[9];
				ucbuff[3] = buff[10];
				ucbuff[4] = buff[11];
				avr_buffer[0] = pack64(ucbuff);
				xQueueSend(Send7200Handle, avr_buffer, 0);
			}
			ucbuff[0] = SEND_RT_FPGA_STATUS;
			ucbuff[1] = buff[12];
			ucbuff[2] = buff[13];
			ucbuff[3] = buff[14];
			ucbuff[4] = buff[15];
			avr_buffer[0] = pack64(ucbuff);
			xQueueSend(Send7200Handle, avr_buffer, 0);

			//memset(buff,0x30,sizeof(buff));
		}else
		{
//			buff[frame_ptr] = (xbyte & 0x7F);		// test for broken wire (in this case bit 7)
			buff[frame_ptr] = xbyte;
			frame_ptr++;
		}
		HAL_GPIO_WritePin(GPIOA, PP_ACK_Pin, GPIO_PIN_SET);
		vTaskDelay(5);
		HAL_GPIO_WritePin(GPIOA, PP_ACK_Pin, GPIO_PIN_RESET);
		xbyte = 0;
		}else vTaskDelay(1000);
	}
  /* USER CODE END StartRecvFPGA */
}

/* USER CODE BEGIN Header_StartKeyStateTask */
/**
* @brief Function implementing the myTask10 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartKeyStateTask */
void StartKeyStateTask(void const * argument)
{
  /* USER CODE BEGIN StartKeyStateTask */
 	static UCHAR key;
	unsigned long sendval;
	uint64_t avr_buffer[5];
	UCHAR ucbuff[10];
	e_isrState = STATE_WAIT_FOR_PRESS;

	vTaskDelay(1000);
  /* Infinite loop */

	for(;;)
	{
		switch (e_isrState)
		{
			case STATE_WAIT_FOR_PRESS:
				key = do_keyscan();
				if(key != 0)
				{
					e_isrState = STATE_WAIT_FOR_RELEASE;
					if(key_mode == NORMAL)
					{
						ucbuff[0] = DTMF_TONE_ON;
						if(key == KP_0)
							ucbuff[1] = 0;
						else
							ucbuff[1] = key-KP_1+1;
						avr_buffer[0] = pack64(ucbuff);
						xQueueSend(SendFPGAHandle,avr_buffer,0);
					}
				}
				break;

			case STATE_WAIT_FOR_RELEASE:
				//keypad released
				if(key_is_released() == 1)
				{
					e_isrState = STATE_WAIT_FOR_PRESS;
					drive_row_high();
					sendval = (unsigned long)key;
					xQueueSend(keypressedHandle, &sendval, 0);
					if(key_mode == NORMAL)
					{
						ucbuff[0] = DTMF_TONE_OFF;
						avr_buffer[0] = pack64(ucbuff);
						vTaskDelay(5);
						xQueueSend(SendFPGAHandle,avr_buffer,0);
					}
					key = 0;
				}
				break;
			default:
				e_isrState = STATE_WAIT_FOR_PRESS;
				break;
		}
		vTaskDelay(10);
	}
  /* USER CODE END StartKeyStateTask */
}

/* Callback01 function */
void Callback01(void const * argument)
{
  /* USER CODE BEGIN Callback01 */

	if(server_up == 1)
	{
#if 0
		if(password_countdown > 0)
		{
			if(password_countdown == 1)
			{
				global_cmd = SHUTDOWN;
				global_ucbuff[0] = global_cmd;
				global_avr_buffer[0] = pack64(global_ucbuff);
				xQueueSend(Send7200Handle, global_avr_buffer, 0);
				engine_on = 0;
				vTaskDelay(100);
				global_ucbuff[0] = DTMF_TONE_OFF;
				global_avr_buffer[0] = pack64(global_ucbuff);
				xQueueSend(SendFPGAHandle,global_avr_buffer,0);
				key_mode = NORMAL;
			}
			password_countdown--;

			if((password_countdown % 2) == 0)
			{
				global_ucbuff[0] = DTMF_TONE_ON;
				global_ucbuff[1] = 0;
				global_avr_buffer[0] = pack64(global_ucbuff);
				xQueueSend(SendFPGAHandle,global_avr_buffer,0);
				vTaskDelay(10);
				global_ucbuff[0] = DTMF_TONE_OFF;
				global_avr_buffer[0] = pack64(global_ucbuff);
				xQueueSend(SendFPGAHandle,global_avr_buffer,0);
			}else
			{
				global_ucbuff[0] = DTMF_TONE_ON;
				global_ucbuff[1] = 1;
				global_avr_buffer[0] = pack64(global_ucbuff);
				xQueueSend(SendFPGAHandle,global_avr_buffer,0);
				vTaskDelay(10);
				global_ucbuff[0] = DTMF_TONE_OFF;
				global_avr_buffer[0] = pack64(global_ucbuff);
				xQueueSend(SendFPGAHandle,global_avr_buffer,0);
			}
		}
#endif

		// do the DS1620 temp conversion every second
		// raw_data is engine temp

		writeByteTo1620(DS1620_CMD_STARTCONV);
		vTaskDelay(30);
		raw_data = readTempFrom1620();
		vTaskDelay(30);
		writeByteTo1620(DS1620_CMD_STOPCONV);
		vTaskDelay(100);

		temp_raw = raw_data;	// use temp_raw because it gets butcherd
		global_ucbuff[0] = ENGINE_TEMP;
		global_ucbuff[2] = (UCHAR)temp_raw;
		temp_raw >>= 8;
		global_ucbuff[1] = (UCHAR)temp_raw;		// send high byte 1st
		global_avr_buffer[0] = pack64(global_ucbuff);
		xQueueSend(Send7200Handle, global_avr_buffer, 0);
		vTaskDelay(100);

		// raw_data2 is indoor temp
		writeByteTo16202(DS1620_CMD_STARTCONV);
		vTaskDelay(30);
		raw_data2 = readTempFrom16202();
		vTaskDelay(30);
		writeByteTo16202(DS1620_CMD_STOPCONV);
		vTaskDelay(100);

		temp_raw = raw_data2;
		global_ucbuff[0] = INDOOR_TEMP;
		global_ucbuff[2] = (UCHAR)temp_raw;
		temp_raw >>= 8;
		global_ucbuff[1] = (UCHAR)temp_raw;		// send high byte 1st
		global_avr_buffer[0] = pack64(global_ucbuff);
		xQueueSend(Send7200Handle, global_avr_buffer, 0);
		vTaskDelay(100);

/*
257.0 = 250
256.1 = 249
255.2 = 248
...
34.7 = 3
33.8 = 2
32.9 = 1
31.1 = 511
30.2 = 510
...
-65.2 = 404
-66.1 = 403
if temp is below 32.9 then raw_data returned is
511 down to 403 so just subtract 511 and make it 
a negative number (31.1 = 0, 30.2 = -1 ... -65.2 = -107,-66.1 = -108
*/

		if(raw_data <= 511 && raw_data >= 403)
		{
			raw_data =- 511;
		}

		if(raw_data2 <= 511 && raw_data2 >= 403)
		{
			raw_data2 =- 511;
		}

		if(blower_on_countdown != 0)
			blower_on_countdown--;

		if(too_high_flag == 0 && raw_data > engine_temp_limit)
		{
			// sound alarm
			global_ucbuff[0] = SPECIAL_TONE_ON;
			global_avr_buffer[0] = pack64(global_ucbuff);
			xQueueSend(SendFPGAHandle,global_avr_buffer,0);
			vTaskDelay(100);
			global_cmd = TEMP_TOO_HIGH;
			global_ucbuff[0] = global_cmd;
			global_avr_buffer[0] = pack64(global_ucbuff);
			xQueueSend(Send7200Handle, global_avr_buffer, 0);
			too_high_flag = 1;

		}else if(too_high_flag == 1)		// must be else or else
		{
			global_ucbuff[0] = DTMF_TONE_OFF;
			global_avr_buffer[0] = pack64(global_ucbuff);
			xQueueSend(SendFPGAHandle,global_avr_buffer,0);
			too_high_flag = 0;
		}

		if(engine_on > 0 && fan_on == 0 && raw_data >= fan_on_temp)
		{
			global_cmd = ON_FAN;
			global_ucbuff[0] = global_cmd;
			fan_on = 1;
			global_avr_buffer[0] = pack64(global_ucbuff);
			xQueueSend(Send7200Handle, global_avr_buffer, 0);
			vTaskDelay(200);
		}

		if(cooling_fan_override == 0 && fan_on == 1 && raw_data <= fan_off_temp)
		{
			global_cmd = OFF_FAN;
			global_ucbuff[0] = global_cmd;
			fan_on = 0;
			global_avr_buffer[0] = pack64(global_ucbuff);
			xQueueSend(Send7200Handle, global_avr_buffer, 0);
		}
#if 0
//currently have the blower on a 'hard-wired' switch
		if((raw_data > blower_en_temp && engine_on > 0) && blower_on_countdown == 0)
//			if(raw_data > blower_en_temp && engine_on > 0)
		{
			if(blower_override == 0 && raw_data2 < blower3_temp && blower_on != 3)
			{
				global_cmd = BLOWER3;
				global_ucbuff[0] = global_cmd;
				global_avr_buffer[0] = pack64(global_ucbuff);
				xQueueSend(Send7200Handle, global_avr_buffer, 0);
				blower_on = 3;

			}else if(blower_override == 0 && raw_data2 < blower2_temp 
					&& raw_data2 > blower3_temp && blower_on != 2)
			{
				global_cmd = BLOWER2;
				global_ucbuff[0] = global_cmd;
				global_avr_buffer[0] = pack64(global_ucbuff);
				xQueueSend(Send7200Handle, global_avr_buffer, 0);
				blower_on = 2;

			}else if(blower_override == 0 && raw_data2 < blower1_temp 
					&& raw_data2 > blower2_temp && blower_on != 1)
			{
				global_cmd = BLOWER1;
				global_ucbuff[0] = global_cmd;
				global_avr_buffer[0] = pack64(global_ucbuff);
				xQueueSend(Send7200Handle, global_avr_buffer, 0);
				blower_on = 1;

			}else if(raw_data2 > blower1_temp && blower_on != 0)
			{
				global_cmd = BLOWER_OFF;
				global_ucbuff[0] = global_cmd;
				global_avr_buffer[0] = pack64(global_ucbuff);
				xQueueSend(Send7200Handle, global_avr_buffer, 0);
				blower_on = 0;
			}
		}
#endif
/*
if(test_rise_temp == 0)
	raw_data += 2;				// used for testing
else raw_data -= 2;
temp_raw = raw_data;

if(raw_data > 209)
	test_rise_temp = 1;

if(raw_data < 121)
	test_rise_temp = 0;
*/
//#if 0
		if(timer_toggle == 0)
		{
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
			timer_toggle = 1;
		}else
		{
			HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
			timer_toggle = 0;
		}
//#endif
	}	// end of 'if server up'
  /* USER CODE END Callback01 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//	UCHAR cmd;

	if(huart->Instance == USART1)
	{
	}
}
uint64_t pack64(UCHAR *buff)
{
	uint64_t var64;

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
