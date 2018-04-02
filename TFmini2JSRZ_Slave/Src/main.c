/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "bsp.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
//#define I2C_ADDRESS 0x0059

uint8_t i2cRxBuffer[2];
uint8_t i2cBuffer[4];

uint8_t rx1Buffer[1];	//串口1 Buffer

softTimer softTimer0 = {0, 0};
softTimer softTimer1 = {0, 0};

TFmini tfminiOne = {0, 0, 0};

uint32_t adcValue[10];

//距离阈值
uint32_t distTV = 200;	//distance threshold value = 200cm
uint32_t RP = 0;	//电位器adc值对应的cm. 
#define TV_S 200
#define TV_M 300
#define TV_L 500

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
	for(uint32_t j = 0; j < 2000000L; j++){}
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_ADC_Init();
  MX_TIM14_Init();

  /* USER CODE BEGIN 2 */
	
	//I2C
	//i2cReadConfig(I2C_ADDRESS);
	//HAL_I2C_Slave_Transmit_IT(&hi2c1, (uint8_t*)i2cBuffer, 4);	
	
	//TIM14, Timer Clock, 48MHz(48 000 000)
	//Prescaler 4800 - 1, Counter Period 10 - 1, 定时1ms
	HAL_TIM_Base_Start_IT(&htim14);	//开中断
	
	//adc0
	HAL_ADC_Start_DMA(&hadc, adcValue, 10);
	
	//HOUT
	HAL_GPIO_WritePin(HOUT_GPIO_Port, HOUT_Pin, GPIO_PIN_SET);	//初始HOUT输出高
	ledOff();
	
		//softTimer: 1s
//	softTimer0.msSet = 100;
//	softTimer0.enable = 1;	
	
	//串口测试, 115200bps, TX-PC, RX-TFmini	
	//printf("Hello, JSRZ!\r\n");
	HAL_UART_Receive_IT(&huart1, (uint8_t *)rx1Buffer, 1);
	
	HAL_Delay(22);
	
	//I2C
	HAL_I2C_Slave_Receive_IT(&hi2c1, i2cRxBuffer, 1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */	
		
		//每100ms获取一次电位器的值, 打印距离和阈值
		if(softTimer0.enable == 0) {		
			softTimer0.msSet = 100;
			softTimer0.enable = 1;
			
			uint32_t adc0 = 0;
			uint32_t i = 0;
			for(i = 0; i < 10;) {
				adc0 += adcValue[i++];
			}
			adc0 /= 10;
			RP = adc0 * 1200 / 4096;	//cm, 0~1200
			RP = RP < 30 ? 30 : RP;	//cm, 30~1200
					
		}	//end softTimer0
		
		
		//根据拨码开关设定阈值
		if(HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin) == GPIO_PIN_SET) {	//off 0
			if(HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin) == GPIO_PIN_SET) {	//00
				//电位器的值
				distTV = RP;
			} else {	//01
				distTV = TV_S;
			}
		} else {
			if(HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin) == GPIO_PIN_SET) {	//10
				distTV = TV_M;
			} else {	//11
				distTV = TV_L;
			}
		}	
		
		
		
		//TFmini数据测试
		if(tfminiOne.receiveComplete == 1) {
			tfminiOne.receiveComplete = 0;
			//printf("(%d, %d)\r\n", tfminiOne.distance, tfminiOne.strength);
			//printf("(%d, %d)\r\n", tfminiOne.distance, distTV);
			i2cBuffer[0] = tfminiOne.distance % 256;
			i2cBuffer[1] = tfminiOne.distance / 256;
			i2cBuffer[2] = tfminiOne.strength % 256;
			i2cBuffer[3] = tfminiOne.strength / 256;
			//printf("(distance: %d cm, threshold: %d cm, %d)\r\n", tfminiOne.distance, distTV, i2cBuffer[0]);
			//printf("(%d, %d)\r\n", tfminiOne.distance, distTV);
		} else if(softTimer0.msSet == 50 && tfminiOne.distance == 0) {
			//printf("(%d, %d)\r\n", tfminiOne.distance, distTV);
			//printf("(distance: %d cm, threshold: %d cm)\r\n", tfminiOne.distance, distTV);
		} else {
		}			
		
		
		//阈值处理
		//大于阈值输出低, 小于阈值输出高.
		//TFmini: 30~1200cm, 10cm用来消抖
		static uint8_t outStatus = 0;
		if(tfminiOne.distance > 30 && tfminiOne.distance > distTV + 15) {
			outStatus = 1;
			HAL_GPIO_WritePin(HOUT_GPIO_Port, HOUT_Pin, GPIO_PIN_RESET);	//HOUT输出低
			ledOn();
			softTimer1.msSet = 200;	
			softTimer1.enable = 1;
		} else if(softTimer1.enable == 0 && outStatus == 1 && tfminiOne.distance < distTV + 5) {
			outStatus = 0;
			HAL_GPIO_WritePin(HOUT_GPIO_Port, HOUT_Pin, GPIO_PIN_SET);	//HOUT输出高
			ledOff();
		} else {
		}
		
  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

//I2C中断
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	UNUSED(hi2c);
	if(hi2c == &hi2c1) {
		if(i2cRxBuffer[0] == 0x42) {
			//while(HAL_I2C_GetState(&hi2c1) != HAL_I2C_STATE_READY){}
			HAL_I2C_Slave_Transmit(&hi2c1, i2cBuffer, 4, 10);
		}
		
	}
	HAL_I2C_Slave_Receive_IT(&hi2c1, i2cRxBuffer, 1);
}

//串口中断
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	UNUSED(huart);
	
	if(huart == &huart1) {
		__HAL_UART_CLEAR_PEFLAG(&huart1);
		HAL_UART_Receive_IT(&huart1, rx1Buffer, 1);
		//HAL_UART_Transmit(&huart1, rx1Buffer, 1, 10);	//echo
		getTFminiOneData(&tfminiOne, rx1Buffer[0]);
	}	//end huart1
}

//htim14中断, 1ms一次
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if(htim == &htim14) {
		if((softTimer0.enable==1) && (softTimer0.msSet>0)) {
			--softTimer0.msSet;
			if(softTimer0.msSet==0) {
				softTimer0.enable = 0;
			}
		}	//softTimer0 end
		if((softTimer1.enable==1) && (softTimer1.msSet>0)) {
			--softTimer1.msSet;
			if(softTimer1.msSet==0) {
				softTimer1.enable = 0;
			}
		}	//softTimer1 end
	}	//end htim14
}

//拨码开关 上升下降沿中断, 这里没有用
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if(GPIO_Pin == SW1_Pin) {
	} else if(GPIO_Pin == SW2_Pin)  {
	} else {
	}
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
