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
