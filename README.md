# TFmini2JSRZ
TFmini, UART to I2C, Switching Value. STM32F030F4P6 is I2C Slave. Arduino is I2C Master.  

- [PCB](#pcb)  
- [Threshold](#threshold)  
- [Program](#program)  



## PCB  
Schematic diagram: [TFmini2JSRZ_SCH.pdf](/TFmini2JSRZ_SCH.pdf)  
PCB Gerber: [TFmini2JSRZ Gerber Files](/TFmini2JSRZ_Gerber_Files)  
Instructions for making boards: [TFmini2JSRZ制板说明.xls](/TFmini2JSRZ制板说明.xls), **Note that the board thickness is 1.0mm**.  
List of components: [TFmini2JSRZ _BOM.xls](/TFmini2JSRZ_BOM.xls), All components can be purchased in [立创商城](http://www.szlcsc.com/), [1688](https://www.1688.com/)或者 [淘宝](https://www.taobao.com/) .  

**After testing, need to get rid of D1 and changed to 0 Ω resistance, otherwise, TFmini cannot start normally under 3.9v.**  

The appearance of finished boards:  
![TFmini2JSRZ](/Assets/TFmini2JSRZ.png)  



## Threshold
HOUT is the switch pin, push pull output 3.3v level. If you want to connect 5V level, you can modify the program, change HOUT pin to open leakage output, pull up 10K resistance to 5V.    

The output of TFmini ranging beyond the range threshold is low (0V), while the output below the range threshold is high (3.3v).  

In order to prevent the output from jitter around the threshold, in fact, when the TFmini distance exceeds the distance threshold +15cm, the output is low (0V, the indicator is D3 on), while when the distance threshold is lower than +5cm, the output is high (3.3v, the indicator is D3 off).

The threshold value of switch quantity (HOUT) can be set by dialing code switch (dial On, 1 above) and potentiometer: 


Dial switch | threshold
---------|----------
 00(Off Off) | Through the potentiometer adjustment (0.3~12m), the clockwise threshold increases, observe the LED indicator light on and off to set the appropriate threshold 
 01(Off On) | 2m 
 10(On Off) | 3m 
 11(On On) | 5m 

You can directly use j-link or st-link to burn B files under A.

## Program
The program for the TFmini2JSRZ board is [TFmini2JSRZ_Slave](/TFmini2JSRZ_Slave). You can directly use J-LINK or ST-LINK to burn  `TFmini2JSRZ_Slave.hex`  files under  `TFmini2JSRZ\TFmini2JSRZ_Slave\MDK-ARM\TFmini2JSRZ_Slave` .

I2C Slave address is 0x59(89), write address is 178, read address is 179(0x59 << 1 + 1). Master sends 0x42(66) to Slave(TFmini2JSRZ), Slave(TFmini2JSRZ) returns 4 bytes each time, namely 8 bits low of distance, 8 bits high of distance, 8 bits low of strength, 8 bits high of strength.  

The I2C master-slave communication data captured by the logic analyzer are as follows:  

![logic](/Assets/logic.png)  

Take Arduino as an example of I2C host, and the connection relationship is as follows:


TFmini2JSRZ | Arduino 
---------|----------
 GND | GND 
 SCL | SCL 
 SDA | SDA 
 
The reading example is as follows: 

```Arduino
#include <Wire.h>

#define I2C_SLAVE_ADDR1  0x59 

typedef struct {
  char distanceLow;
  char distanceHigh;
  int distance;
  char strengthLow;
  char strengthHigh;
  int strength;
  boolean receiveComplete;
}TFmini;

TFmini TFminiOne = {0, 0, 0, 0, 0, 0, false};

void setup() {
  Wire.begin();   // join i2c bus (address optional for master)
  Serial.begin(115200);
}

unsigned long lastTime = millis();
unsigned int count = 0;
unsigned int frequency = 0;

void loop() {


  if((millis() - lastTime) % 10 == 0) {
    delay(1); //I2C Communication < 1ms. It will be run twice per 10 milliseconds if not delay(1)
    
    Wire.beginTransmission(I2C_SLAVE_ADDR1);
    Wire.write(byte(0x42));
    Wire.endTransmission();
  
    Wire.requestFrom(I2C_SLAVE_ADDR1, 4);    // request 4 bytes from slave device 
    if(4 <= Wire.available()) {
      TFminiOne.distanceLow = Wire.read();
      TFminiOne.distanceHigh = Wire.read();
      TFminiOne.strengthLow = Wire.read();
      TFminiOne.strengthHigh = Wire.read(); 
      TFminiOne.receiveComplete = true;
    }
  }
  
  if(TFminiOne.receiveComplete == true) {
    TFminiOne.receiveComplete = false;
    ++count;
    
    TFminiOne.distance = TFminiOne.distanceLow + TFminiOne.distanceHigh * 256;
    TFminiOne.strength = TFminiOne.strengthLow + TFminiOne.strengthHigh * 256;
    
    Serial.print(TFminiOne.distance);
    Serial.print("cm\t");
    Serial.print("strength: ");
    Serial.print(TFminiOne.strength);
    Serial.print("\t");
    Serial.print(frequency);
    Serial.println("Hz");
  }   

  if(millis() - lastTime > 999) {
      lastTime = millis();
      frequency = count;
      count = 0;
  }

}

```   

Data are shown in the figure. The left is the actual distance, the middle is the signal quality, and the right is the communication frequency:

![Arduino](/Assets/arduino.png)  
