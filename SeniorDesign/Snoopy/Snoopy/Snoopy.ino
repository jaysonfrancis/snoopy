#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "BMA250.h"
#include <SD.h>
#include <SPI.h>

BMA250 accel;

static const int RXPin = A1, TXPin = A0;
static const uint32_t GPSBaud = 9600;
static const int GPS_ONOFFPin = A3;
static const int GPS_SYSONPin = A2;
static const int chipSelect = 10;
// The TinyGPS++ object
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  //defining pin types for GPS module
  pinMode(GPS_SYSONPin, INPUT);
  pinMode(GPS_ONOFFPin, OUTPUT);
  digitalWrite(GPS_ONOFFPin,LOW);
  delay(5);
  
  //check to see if module is on or off
  if(digitalRead(GPS_SYSONPin) == LOW){
   //wake device up by sending a high then low pulse to GPS ONOFFPin
    digitalWrite(GPS_ONOFFPin,HIGH);
    delay(5);
    digitalWrite(GPS_ONOFFPin, LOW); 
  }  
  
  Serial.begin(115200); //may need to change to 9600 baud
    ss.begin(GPSBaud);
  Wire.begin();
  accel.begin(BMA250_range_16g, BMA250_update_time_05ms);//This sets up the BMA250 accelerometer

  
  // Following section is for SD Write/Read
  pinMode(10, OUTPUT);
  
  if(!SD.begin(chipSelect)){
  return; //card error 
  }  
  Serial.println("card initialized");
  Serial.println(F("Device Testing of GPS module"));
  Serial.println();
  // End of SD Write/Read
  
}

void loop()
{
  
  File dataFile = SD.open("output.csv", FILE_WRITE);
  
  if(dataFile){
    accel.read();
    dataFile.print(accel.X);
    dataFile.print(",");
    dataFile.print(accel.Y);
    dataFile.print(",");
    dataFile.print(accel.Z);
    dataFile.println("");
    dataFile.close();
   
  }  
  
  /*
  accel.read();//This function gets new data from the accelerometer
  Serial.print("X = ");
  Serial.print(accel.X);
  Serial.print("  ");
  Serial.print("Y = ");
  Serial.print(accel.Y);
  Serial.print("  ");
  Serial.print("Z = ");
  Serial.print(accel.Z);
  Serial.print("  Temperature(C) = ");
  Serial.println((accel.rawTemp*0.5)+24.0,1);
  delay(250);//We'll make sure we're over the 64ms update time set on the BMA250
  */
}
