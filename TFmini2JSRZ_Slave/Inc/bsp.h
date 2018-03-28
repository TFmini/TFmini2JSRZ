#ifndef __BSP_H__
#define __BSP_H__

#include "main.h"
#include "stm32f0xx_hal.h"
#include "gpio.h"

//Soft Timer
typedef struct {
	int32_t enable;		//0: disable. 1: enable. 
	uint32_t msSet;		//ms
}softTimer;

//TFmini
typedef struct {
  int distance;
  int strength;
  uint8_t receiveComplete;
}TFmini;
void getTFminiOneData(TFmini* tfmini, char rxBuffer);

//LED
void ledOn(void);
void ledOff(void);
void ledToggle(void);
	

#endif
