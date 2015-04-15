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

const int Buffer = 9;
volatile int cycle; 

volatile double a_x[Buffer];
volatile double a_y[Buffer];
volatile double a_z[Buffer];


int count;
double lat,lng;
int hour,minute,second,centisecond;
// The TinyGPS++ object
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  //defining pin types for GPS module
//  pinMode(GPS_SYSONPin, INPUT);
//  pinMode(GPS_ONOFFPin, OUTPUT);
//  digitalWrite(GPS_ONOFFPin,LOW);
//  delay(5);
  
  //check to see if module is on or off
//  if(digitalRead(GPS_SYSONPin) == LOW){
   //wake device up by sending a high then low pulse to GPS ONOFFPin
//    digitalWrite(GPS_ONOFFPin,HIGH);
//    delay(5);
//    digitalWrite(GPS_ONOFFPin, LOW); 
//  }  
  
  Serial.begin(115200); //may need to change to 9600 baud
//  ss.begin(GPSBaud);
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
  
  cycle = 0;
}









void loop()
{ 
// File dataFile = SD.open("OUTPUT.csv", FILE_WRITE);

ReadValues();
cycle += 1;

if (cycle >= (Buffer)){
  SDcardWrite();
  cycle = 0;
  }

}






void ReadValues(){

  accel.read();
  
  double x_g = accel.X;
  double y_g = accel.Y;
  double z_g = accel.Z;
  
  a_x[cycle] = x_g;
  a_y[cycle] = y_g;
  a_z[cycle] = z_g;
}






void SDcardWrite(){
  int i = 0;
//  int leng = length + 1;
//  char file[length];

  dataFile = SD.open("OUTPUT.csv", FILE_WRITE);

  while (i < cycle){
   if(dataFile){
    dataFile.print(",");
    dataFile.print(a_x[i], DEC);
    dataFile.print(",");
    dataFile.print(a_y[i], DEC);
    dataFile.print(",");
    dataFile.print(a_z[i], DEC);
    dataFile.print(",");
    
    Serial.print(",");
    Serial.print(accel.X);
    Serial.print(",");
    Serial.print(accel.Y);
    Serial.print(",");
    Serial.print(accel.Z);
    Serial.print(",");
    
    
    }
  i += 1;
  }
  dataFile.close();
  return;
}
