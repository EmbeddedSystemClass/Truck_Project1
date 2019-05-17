/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef unsigned char UCHAR;
typedef unsigned int UINT;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define col0_Pin GPIO_PIN_0
#define col0_GPIO_Port GPIOC
#define col1_Pin GPIO_PIN_1
#define col1_GPIO_Port GPIOC
#define col2_Pin GPIO_PIN_2
#define col2_GPIO_Port GPIOC
#define col3_Pin GPIO_PIN_3
#define col3_GPIO_Port GPIOC
#define row3_Pin GPIO_PIN_12
#define row3_GPIO_Port GPIOB
#define row2_Pin GPIO_PIN_13
#define row2_GPIO_Port GPIOB
#define row1_Pin GPIO_PIN_14
#define row1_GPIO_Port GPIOB
#define row0_Pin GPIO_PIN_15
#define row0_GPIO_Port GPIOB
#define LD4_Pin GPIO_PIN_8
#define LD4_GPIO_Port GPIOC
#define LD3_Pin GPIO_PIN_9
#define LD3_GPIO_Port GPIOC
#define TMS_SWDIO_Pin GPIO_PIN_13
#define TMS_SWDIO_GPIO_Port GPIOA
#define TCK_SWCLK_Pin GPIO_PIN_14
#define TCK_SWCLK_GPIO_Port GPIOA
#define Wiper_Park_Switch_Pin GPIO_PIN_4
#define Wiper_Park_Switch_GPIO_Port GPIOC
#define Wiper_Park_Switch_EXTI_IRQn EXTI4_IRQn
#define DataReady_Pin GPIO_PIN_4
#define DataReady_GPIO_Port GPIOA
#define CmdParam_Pin GPIO_PIN_5
#define CmdParam_GPIO_Port GPIOA
#define DataSent_Pin GPIO_PIN_6
#define DataSent_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
