#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "BMA250.h"
#include "DSRTCLib.h"
#include <SD.h>
#include <SPI.h>

//accelerometer instantiation
BMA250 accel;
//RTC instantiation
DS1339 RTC = DS1339();


File dataFile;
//globals for GPS module
static const int RXPin = A1, TXPin = A0;
static const int GPS_ONOFFPin = A3;
static const int GPS_SYSONPin = A2;
static const uint32_t GPSBaud = 9600;
//constants for SD card
static const byte chipSelect = 10;

const int Buffer = 9;
volatile int cycle; 
volatile int readgpscount = 20000;

volatile double a_x[Buffer];
volatile double a_y[Buffer];
volatile double a_z[Buffer];
volatile double lat, lng;

//int hour,minute,second,centisecond;
// The TinyGPS++ object
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
//====================================================================================
//added time stamp functionality
byte ledPin =  13;    // LED connected to digital pin 13
byte INT_PIN = 3; // INTerrupt pin from the RTC. On Arduino Uno, this should be mapped to digital pin 2 or pin 3, which support external interrupts
byte int_number = 1; // On Arduino Uno, INT0 corresponds to pin 2, and INT1 to pin 3

//====================================================================================
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
  
  RTC.start(); //ensure RTC oscillator is running if not already
  
  cycle = 0;
}

void loop()
{ 


ReadValues();

if (readgpscount = 20000){
 ReadGPS(); 
}
cycle += 1;

if (cycle >= (Buffer)){
  SDcardWrite();
  cycle = 0;
  }

}


void ReadGPS() {
   while (ss.available() > 0){
    if (gps.encode(ss.read())){
      if(gps.location.isValid()){
        lat = gps.location.lat();
        lng = gps.location.lng();
      }else{
        lat = 0;
        lng = 0;
      }  
    }//end if gps.encode
  }//end while ss avail
  readgpscount = 0;
 }



void ReadValues(){

  accel.read();
  
  double x_g = accel.X;
  double y_g = accel.Y;
  double z_g = accel.Z;
  
  a_x[cycle] = x_g;
  a_y[cycle] = y_g;
  a_z[cycle] = z_g;
  
  readgpscount++;
}

void SDcardWrite(){
  int i = 0;
//  int leng = length + 1;
//  char file[length];

  dataFile = SD.open("OUTPUT.csv", FILE_WRITE);

  while (i < cycle){
   if(dataFile){
    //dataFile.print(int(RTC.getHours()));
    //dataFile.print(":");
    //dataFile.print(int(RTC.getMinutes()));
    //dataFile.print(":");
    //dataFile.print(int(RTC.getSeconds()));
    //dataFile.print(",");
    dataFile.print(a_x[i], DEC);
    dataFile.print(",");
    dataFile.print(a_y[i], DEC);
    dataFile.print(",");
    dataFile.print(a_z[i], DEC);
    dataFile.print(",");
    dataFile.print(lat);
    dataFile.print(",");
    dataFile.print(lng);
    dataFile.println();
    
    //Serial.print(int(RTC.getHours()));
    //Serial.print(":");
    //Serial.print(int(RTC.getMinutes()));
    //Serial.print(":");
    //Serial.print(int(RTC.getSeconds()));
    //Serial.print(",");
    Serial.print(accel.X);
    Serial.print(",");
    Serial.print(accel.Y);
    Serial.print(",");
    Serial.print(accel.Z);
    Serial.print(",");
    Serial.print(lat);
    Serial.print(",");
    Serial.print(lng);
    Serial.println();
    
    
    }
  i += 1;
  }
  dataFile.close();
  return;
}
