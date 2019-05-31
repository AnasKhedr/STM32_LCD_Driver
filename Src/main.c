/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LCD.h"
#include "string.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  MyLCD lcd('A',7,6,5,4,3,2);					//set the pins that connect the lcd to the MCU: (connected to port D, D7 of the LCD is connected to pin 7 or PORT D, D6 of the LCD is connected to pin 6 or PORT D, D5 of the LCD is connected to pin 5 or PORT D, D4 of the LCD is connected to RS or PORT D, D7 of the LCD is connected to pin 3 or PORT D, E of the LCD is connected to pin 2 or PORT D)
  lcd.init(2,1,0);							//set up the LCD, the LCD has 2 lines, show the cursor underline , don't blink (those are the same as the default arguments so if you just called init(); it would have had the same parameter, hence same effect)

  char customChar0[] = {0,14,21,23,17,14,0,0};//clock
  char customChar1[] = {10,10,31,31,14,4,4,0};//fesha
  char customChar2[] = {4,4,31,0,14,4,4,0};	//capacitor
  char customChar3[] = {0,10,10,0,17,14,0,0};	//smiley face

	//define new graphs or characters (use this site to create the string array needed: https://www.quinapalus.com/hd44780udg.html
	uint8_t clock_handler=lcd.defineGraph(customChar0);	//clock
	uint8_t fesha_handler=lcd.defineGraph(customChar1);	//fesha
	uint8_t cap_handler  =lcd.defineGraph(customChar2);	//capacitor
	uint8_t Sface_handler=lcd.defineGraph(customChar3);	//smiley face

	lcd.clear();		//clear the display since defineGraph() misses with display
	lcd.home();			//rest display position and remove any shifts
	lcd.setCursor(0,0);	//this is useless since home() restes diplay posithion

	lcd.data();								//must tell the LCD that you'll be sending data() if you want to use writeChar() [writeChar() could be used to send command() as well]
	lcd.writeChar(Sface_handler);			//send a char to the lcd (whether this char is a command() or data() is set by thes two APIs)
	lcd.print(" Hello ");					//print a char* (this is an overloaded function that can handle char, char* and vlist)
	lcd.print(Sface_handler);				//print a char  (this is an overloaded function that can handle char, char* and vlist)
	lcd.setCursor(1,0);						//go to the first pixel of line 1 (second line)
	lcd.print("defined chars: %c %c %c %c",clock_handler,Sface_handler,fesha_handler,cap_handler);//print a vlist very much the same as printf (this is an overloaded function that can handle char, char* and vlist)
	HAL_Delay(300);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	 lcd.shiftR(6,100);					//shift the visible part of the LCD 6 times to the Right with a 100ms delay between each single shift
	HAL_Delay(200);
	lcd.shiftL(6,100);					//shift the visible part of the LCD 6 times to the Right with a 100ms delay between each single shift
	HAL_Delay(200);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV8;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
