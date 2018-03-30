#include "bsp.h"

void getTFminiOneData(TFmini* tfmini, uint8_t rxBuffer) {
	static uint8_t rx[9];
	static uint8_t index = 0;
	static uint16_t checksum = 0;
	rx[index] = rxBuffer;
	if(rx[0] != 0x59) {
		index = 0;
	} else if(index == 1 && rx[1] != 0x59) {
		index = 0;
	} else if(index == 8) {
		checksum = rx[0] + rx[1] + rx[2] + rx[3] + rx[4] + rx[5] + rx[6] + rx[7];
		if(rx[8] == (checksum % 256)) {
			tfmini->distance = rx[2] + rx[3] * 256;
			tfmini->strength = rx[4] + rx[5] * 256;
			tfmini->receiveComplete = 1;
		}
		index = 0;
	} else {
		index++;
	}	
}

void ledOn(void) {
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

void ledOff(void) {
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

void ledToggle(void) {
	HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

//void i2cReadConfig(uint32_t i2cAddress) {
void i2cReadConfig(uint16_t address) {
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00201D2B;
  hi2c1.Init.OwnAddress1 = address * 2;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0xFF;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Analogue filter 
    */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Digital filter 
    */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}
