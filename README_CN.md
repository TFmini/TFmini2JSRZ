# TFmini2JSRZ
TFmini, UART to I2C, Switching Value. STM32F030F4P6 is I2C Slave. Arduino is I2C Master.  

- [PCB](#pcb)  
- [Threshold](#threshold)  
- [Program](#program)  



## PCB  
原理图: [TFmini2JSRZ_SCH.pdf](/TFmini2JSRZ_SCH.pdf)  
PCB Gerber: [TFmini2JSRZ Gerber Files](/TFmini2JSRZ_Gerber_Files)  
制板说明: [TFmini2JSRZ制板说明.xls](/TFmini2JSRZ制板说明.xls), **注意板厚1.0mm**.  
元器件清单: [TFmini2JSRZ _BOM.xls](/TFmini2JSRZ_BOM.xls), 所有元器件可以在 [立创商城](http://www.szlcsc.com/), [1688](https://www.1688.com/)或者 [淘宝](https://www.taobao.com/)采购到 .  

**板子回来后, 经测试, 需要去掉D1, 改成 0 Ω电阻, 否则 3.9V 下TFmini不能正常启动.**  

成品板子的外观:  
![TFmini2JSRZ](/Assets/TFmini2JSRZ.png)  



## Threshold
HOUT为开关量引脚, 推挽输出, 3.3V电平. 如果想要接5V电平, 可以修改程序, 把HOUT引脚改为开漏输出, 上拉10K电阻到5V即可.  

TFmini测距超过距离阈值输出低(0V), 低于距离阈值输出高(3.3V).  

为了防止输出在阈值附近抖动, 实际TFmini测距超过 距离阈值+15cm 输出低(0V, 指示灯D3亮), 低于 距离阈值+5cm 输出高(3.3V, 指示灯D3灭).

开关量(HOUT)的阈值可以通过 拨码开关(拨到上面为On, 1) 和 电位器 设置

拨码开关 | 阈值
---------|----------
 00(Off Off) | 通过电位器调节（0.3~12m），顺时针阈值增大，观察LED指示灯的量灭设置合适的阈值
 01(Off On) | 2m 
 10(On Off) | 3m 
 11(On On) | 5m 



## Program

TFmini2JSRZ板子的程序是 [TFmini2JSRZ_Slave](/TFmini2JSRZ_Slave). 可以直接使用J-LINK或者ST-Link烧录 `TFmini2JSRZ\TFmini2JSRZ_Slave\MDK-ARM\TFmini2JSRZ_Slave`  下的 `TFmini2JSRZ_Slave.hex` 文件.

I2C从机地址 0x59(89), 写地址 178, 读地址是 179(0x59 << 1 + 1). Master向Slave(TFmini2JSRZ)发送 0x42(66), Slave(TFmini2JSRZ)每次返回4个字节, 依次是 distance低8位, distance高8位, strength低8位, strength高8位.

逻辑分析仪抓取的I2C主从通信数据如下:

![logic](/Assets/logic.png)  

以Arduino作I2C主机为例, 连接关系:

TFmini2JSRZ | Arduino 
---------|----------
 GND | GND 
 SCL | SCL 
 SDA | SDA 
 
读取示例如下：

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

数据如图, 左边为实际距离, 中间为信号品质, 右边为通信频率：

![Arduino](/Assets/arduino.png)  
