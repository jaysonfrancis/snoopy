

// MPU-6050 Accelerometer + Gyro
// -----------------------------


#include <Wire.h> // Import I2C Library
#include <SD.h> // Import SD Card Library
#include "LowPower.h"// Import Low Power Library
//#include <stdlib.h> ;

File myFile;

#define MPU6050_I2C_ADDRESS 0x68
#define MPU6050_PWR_MGMT_1         0x6B   // R/W
#define MPU6050_CONFIG             0x1A   // R/W
#define MPU6050_ACCEL_XOUT_H       0x3B   // R
#define MPU6050_ACCEL_CONFIG       0x1C   // R/W
#define MPU6050_GYRO_CONFIG        0x1B   // R/W


typedef union accel_t_gyro_union
{
 struct
 {
   uint8_t x_accel_h;
   uint8_t x_accel_l;
   uint8_t y_accel_h;
   uint8_t y_accel_l;
   uint8_t z_accel_h;
   uint8_t z_accel_l;
   uint8_t t_h;
   uint8_t t_l;
   uint8_t x_gyro_h;
   uint8_t x_gyro_l;
   uint8_t y_gyro_h;
   uint8_t y_gyro_l;
   uint8_t z_gyro_h;
   uint8_t z_gyro_l;
 } reg;
 struct 
 {
   int16_t x_accel;
   int16_t y_accel;
   int16_t z_accel;
   int16_t temperature;
   int16_t x_gyro;
   int16_t y_gyro;
   int16_t z_gyro;
 } value;
};

const int Buffer = 9;

volatile float globaltime;
String index;
volatile int length;

volatile int cycle;

volatile double a_x[Buffer];
volatile double a_y[Buffer];
volatile double a_z[Buffer];

volatile double g_x[Buffer];
volatile double g_y[Buffer];
volatile double g_z[Buffer];

volatile double t[Buffer];
volatile double light[Buffer];

volatile float time[Buffer];


void setup()
{      
int error;
 uint8_t c;


 Serial.begin(115200);

 // Initialize the 'Wire' class for the I2C-bus.
 Wire.begin();




 //Serial.println("Initializing MPU");


 // Clear the 'sleep' bit to start the sensor.
 MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0);
  // Incease low pass filter
 MPU6050_write_reg (MPU6050_CONFIG, 3);

  // Incease Accelerometer range to +- 8g
 MPU6050_write_reg (MPU6050_ACCEL_CONFIG, 16);
 
    // Incease Gyro range to +- 500 deg/sec
 MPU6050_write_reg (MPU6050_GYRO_CONFIG, 8);
 
 //Serial.println("Initializing SD Card");
  pinMode(10, OUTPUT);// CS pin is 10

 
 globaltime = 0;
 
 index = "";
 
   Serial.println("reading index");

   if (!SD.begin(10)) 
   {
     //Serial.println("initialization failed!");
   return;
   }
     
     
     myFile = SD.open("index.txt");
   
   if (myFile) {
   
   // read from the file until there's nothing else in it:
   while (myFile.available()) 
   {
   	char letter = myFile.read();
       index += letter;
   }
   // close the file:
   myFile.close();
   } 
   

   index += 'x';



   myFile = SD.open("index.txt", FILE_WRITE);
   
   Serial.println(myFile);
   
   myFile.print('x');

   myFile.close();

 length = index.length();
 index = (String)(index.length());
 
 index = index + ".csv";

 length = index.length();
 
 int leng = length + 1;
 char file[length];
 index.toCharArray(file,leng); 
       
 delay(5);

 myFile = SD.open(file, FILE_WRITE);
   Serial.println(myFile);

 delay(5);
 myFile.println("The Product,Time since start (ms),Accel_x (g),Accel_y (g),Accel_z (g),Gyro_x (deg/s),Gyro_y (deg/s),Gyro_z (deg/s),Temperature (F), Light");
 delay(5);
 myFile.close();	// close the file
 
 cycle = 0;
 }


void loop()
 {
 
 MPU6050_write_reg (MPU6050_PWR_MGMT_1, 0);// Wake Up MPU
 
 delay(40);
 ReadValues();
 
 
 MPU6050_write_reg (MPU6050_PWR_MGMT_1, 64);// Sleep MPU
 cycle += 1;
   
 if (cycle >= (Buffer))
 {

   SDcardWrite();
   cycle = 0;
 }

 LowPower.powerDown(SLEEP_120MS, ADC_OFF, BOD_OFF);
 
 }
 
 void ReadValues()
 {
 int error;
 double temp;
 accel_t_gyro_union accel_t_gyro;
 
 //Serial.println(F(""));
 //Serial.println(F("MPU-6050 reading"));

 // Read the raw values.
 // Read 14 bytes at once, 
 // containing acceleration, temperature and gyro.
 // With the default settings of the MPU-6050,
 // there is no filter enabled, and the values
 // are not very stable.
 error = MPU6050_read (MPU6050_ACCEL_XOUT_H, (uint8_t *) &accel_t_gyro, sizeof(accel_t_gyro));


 // Swap all high and low bytes.
 // After this, the registers values are swapped, 
 // so the structure name like x_accel_l does no 
 // longer contain the lower byte.
 
  uint8_t swap;
 #define SWAP(x,y) swap = x; x = y; y = swap

 SWAP (accel_t_gyro.reg.x_accel_h, accel_t_gyro.reg.x_accel_l);
 SWAP (accel_t_gyro.reg.y_accel_h, accel_t_gyro.reg.y_accel_l);
 SWAP (accel_t_gyro.reg.z_accel_h, accel_t_gyro.reg.z_accel_l);
 SWAP (accel_t_gyro.reg.t_h, accel_t_gyro.reg.t_l);
 SWAP (accel_t_gyro.reg.x_gyro_h, accel_t_gyro.reg.x_gyro_l);
 SWAP (accel_t_gyro.reg.y_gyro_h, accel_t_gyro.reg.y_gyro_l);
 SWAP (accel_t_gyro.reg.z_gyro_h, accel_t_gyro.reg.z_gyro_l);


 // Print the raw acceleration values

 double x_g = accel_t_gyro.value.x_accel;
 double y_g = accel_t_gyro.value.y_accel;
 double z_g = accel_t_gyro.value.z_accel;
 
 a_x[cycle] = x_g/4096;
 a_y[cycle] = y_g/4096;
 a_z[cycle] = z_g/4096;
 

 temp = ( (double) accel_t_gyro.value.temperature + 12412.0) / 340.0;
 t[cycle] = (temp)*9/5 + 32;// Convert C to F

 // Print the raw gyro values.

 double x_s = accel_t_gyro.value.x_gyro;
 double y_s = accel_t_gyro.value.y_gyro;
 double z_s = accel_t_gyro.value.z_gyro;
 
 g_x[cycle] = x_s/65.5;
 g_y[cycle] = y_s/65.5;
 g_z[cycle] = z_s/65.5;
 

 light[cycle] = analogRead(A0);

   float clock = millis();
   globaltime = globaltime + 120;
   time[cycle] = ((globaltime + clock)/1000) * 1.0464;
   


}

void SDcardWrite()
{
   
 int i = 0;
 
 int leng = length + 1;
 char file[length];
 index.toCharArray(file,leng); 
 
 myFile = SD.open(file, FILE_WRITE);
 
  
 while (i < cycle)
{  
 if (myFile) {
   
   // Store in CSV format
   
   // Accel values
   myFile.print(",");   
   myFile.print(time[i]);
   myFile.print(",");   
   myFile.print(a_x[i], DEC);
   myFile.print(",");
   myFile.print(a_y[i], DEC);
   myFile.print(",");
   myFile.print(a_z[i], DEC);

   // Gyro values
   myFile.print(",");
   myFile.print(g_x[i], DEC);
   myFile.print(",");
   myFile.print(g_y[i], DEC);
   myFile.print(",");
   myFile.print(g_z[i], DEC);
   
   myFile.print(",");
   myFile.print(t[i], DEC);

   myFile.print(",");
   myFile.print(light[i], DEC);
   myFile.println("");
   
   //Serial.println("done.");
 } 

 i += 1;
}
 myFile.close();

 return;
}

// --------------------------------------------------------
// MPU6050_read
//
// This is a common function to read multiple bytes 
// from an I2C device.
//
// It uses the boolean parameter for Wire.endTransMission()
// to be able to hold or release the I2C-bus. 
// This is implemented in Arduino 1.0.1.
//
// Only this function is used to read. 
// There is no function for a single byte.
//
int MPU6050_read(int start, uint8_t *buffer, int size)
{
 int i, n, error;

 Wire.beginTransmission(MPU6050_I2C_ADDRESS);
 n = Wire.write(start);
 if (n != 1)
   return (-10);

 n = Wire.endTransmission(false);    // hold the I2C-bus
 if (n != 0)
   return (n);

 // Third parameter is true: relase I2C-bus after data is read.
 Wire.requestFrom(MPU6050_I2C_ADDRESS, size, true);
 i = 0;
 while(Wire.available() && i<size)
 {
   buffer[i++]=Wire.read();
 }
 if ( i != size)
   return (-11);

 return (0);  // return : no error
}



