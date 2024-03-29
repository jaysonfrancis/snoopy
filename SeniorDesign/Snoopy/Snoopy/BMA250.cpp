#include "BMA250.h"
#include <inttypes.h>
#include "Arduino.h"
#include <Wire.h>

BMA250::BMA250()
{
}

void BMA250::begin(uint8_t range, uint8_t bw)
{
  //Setup the range measurement setting
  Wire.beginTransmission(BMA250_I2CADDR);
  Wire.write(0x0F); 
  Wire.write(range);
  Wire.endTransmission();
  
  //Setup the bandwidth
  Wire.beginTransmission(BMA250_I2CADDR);
  Wire.write(0x10);
  Wire.write(bw);
  Wire.endTransmission();
}


void BMA250::read()
{
  //Set register index
  Wire.beginTransmission(BMA250_I2CADDR);
  Wire.write(0x02);
  Wire.endTransmission();
  
  //Request seven data bytes
  Wire.requestFrom(BMA250_I2CADDR,7);
  
  //Receive acceleration measurements as 16 bit integers
  X = Wire.read();
  X |= Wire.read()<<8;
  Y = Wire.read();
  Y |= Wire.read()<<8;
  Z = Wire.read();
  Z |= Wire.read()<<8;
  
  //The following shifts preserve the sign bits
  X>>=6;Y>>=6;Z>>=6;
  
  //Receive temperature measurement
  rawTemp = Wire.read();
}

