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
int count;
double lat,lng;
int hour,minute,second,centisecond;
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
  accel.begin(BMA250_range_16g, BMA250_update_time_8ms);//This sets up the BMA250 accelerometer

  
  // Following section is for SD Write/Read
  pinMode(10, OUTPUT);
  
  if(!SD.begin(chipSelect)){
  Serial.println("Card failed, or not present");
  return; //card error 
  }  
  Serial.println("card initialized");
  Serial.println();
  // End of SD Write/Read
  
}

void loop()
{ 
  File dataFile = SD.open("OUTPUT.csv", FILE_WRITE);
  // IMPLEMENT FILE SLICING HERE
  
  while (ss.available() > 0){
    if (gps.encode(ss.read())){
      if (gps.time.isValid()) //write to file if time is valid
      {
        hour = gps.time.hour();
        minute = gps.time.minute();
        second = gps.time.second();    
      }//end if gps.time
      else
      {
        hour = 0;
        minute = 0;
        second = 0; 
      }//end else
      
      //check to see if gps location is valid
      if(gps.location.isValid()){
        lat = gps.location.lat();
        lng = gps.location.lng();
      }else{
        lat = gps.location.lat();
        lng = gps.location.lng();
      }
    }//end if gps.encode
  }//end while ss avail
  
  //section below is to verify functionality of both sensors
    accel.read();
    //Serial.print(hour);
    //Serial.print(F(":"));
    //Serial.print(minute);
    //Serial.print(F(":"));
    //Serial.print(second);
    //Serial.print(",");
    Serial.print(count);
    Serial.print(",");
    Serial.print(accel.X);
    Serial.print(",");
    Serial.print(accel.Y);
    Serial.print(",");
    Serial.print(accel.Z);
    Serial.print(",");
    Serial.print(lat);
    Serial.print(",");
    Serial.println(lng);
    Serial.println();
    
  //following section is to write to a datafile
  if(dataFile){
    accel.read();
    dataFile.print(count);
    dataFile.print(",");
    dataFile.print(accel.X);
    dataFile.print(",");
    dataFile.print(accel.Y);
    dataFile.print(",");
    dataFile.print(accel.Z);
    dataFile.print(lat);
    dataFile.print(",");
    dataFile.print(lng);
    dataFile.println();
    dataFile.close();
   
  }  
  count=count+1;
}
