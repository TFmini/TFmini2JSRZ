# TFmini2JSRZ
TFmini, UART to I2C, Switching Value. STM32F030F4P6 is I2C Slave. STM32 Or Arduino is I2C Master.  

- [PCB](#pcb)  
- [Threshold](#threshold)  



## PCB  
原理图: [TFmini2JSRZ_SCH.pdf](/TFmini2JSRZ_SCH.pdf)  
PCB Gerber: [TFmini2JSRZ Gerber Files](/TFmini2JSRZ_Gerber_Files)  
制板说明: [TFmini2JSRZ制板说明.xls](/TFmini2JSRZ制板说明.xls), **注意板厚1.0mm**.  
元器件清单: [TFmini2JSRZ _BOM.xls](/TFmini2JSRZ_BOM.xls), 所有元器件可以在 [立创商城](http://www.szlcsc.com/), [1688](https://www.1688.com/)或者 [淘宝](https://www.taobao.com/) 采购到.  

**板子回来后, 经测试, 需要去掉D1, 改成 0 Ω电阻, 否则 3.9V 下TFmini不能正常启动.**  

成品板子的外观:  
![TFmini2JSRZ](/Assets/TFmini2JSRZ.png)  



## Threshold
HOUT为开关量引脚, 推挽输出, 3.3V电平. 如果想要接5V电平, 可以修改程序, 把HOUT引脚改为开漏输出, 上拉10K电阻到5V即可.    

TFmini测距超过距离阈值输出低(0V), 低于距离阈值输出高(3.3V).  

为了防止输出在阈值附近抖动, 实际TFmini测距超过 距离阈值+15cm 输出低(0V, 指示灯D3亮), 低于 距离阈值+5cm 输出高(3.3V, 指示灯D3灭).  

开关量(HOUT)的阈值可以通过 拨码开关(拨到上面为On, 1) 和 电位器 设置:  


拨码开关 | 阈值 
---------|----------
 00(Off Off) | 通过电位器调节(0.3~12m), 顺时针阈值增大 
 01(Off On) | 2m 
 10(On Off) | 3m 
 11(On On) | 5m 

用电位器设置阈值时(拨码开关拨到00), 可以通过miniU2S板连接到PC查看阈值信息:  

![miniU2S](/Assets/miniU2S.png)  

miniU2S 板载 CP2104 芯片用于USB转串口, CP2104的驱动可以下载安装 [CP210x_Driver](https://cn.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers).  

打开SSCOM, 选择端口号, 波特率115200, 打开串口, 图中的744即为电位器阈值:  

![SSCOM](/Assets/sscom.png)  





