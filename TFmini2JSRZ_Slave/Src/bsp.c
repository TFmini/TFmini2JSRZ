#include "bsp.h"

void getTFminiOneData(TFmini* tfmini, char rxBuffer) {
	static char rx[9];
	static char index = 0;
	static int checksum = 0;
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
