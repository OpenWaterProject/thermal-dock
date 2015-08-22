/*
  SD card datalogger
 
 This example shows how to log data from three analog sensors 
 to an SD card using the SD library.
 	
 The circuit:
 * SD card attached to SPI bus as follows:
 ** UNO:  MOSI - pin 11, MISO - pin 12, CLK - pin 13, CS - pin 4 (CS pin can be changed)
  and pin #10 (SS) must be an output
 ** Mega:  MOSI - pin 51, MISO - pin 50, CLK - pin 52, CS - pin 4 (CS pin can be changed)
  and pin #52 (SS) must be an output
 ** Leonardo: Connect to hardware SPI via the ICSP header
 		Pin 4 used here for consistency with other Arduino examples
 
 created  24 Nov 2010
 modified 9 Apr 2012 by Tom Igoe
 
 This example code is in the public domain.
 	 
 */

#include <SPI.h>
#include <SD.h>
#include <stdlib.h>
#include "Wire.h"

// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
const int chipSelect = 10;

File dataFile;

const int T_a = 0x4F;
const int T_b = 0x48;
const int T_c = 0x4B;
const int T_d = 0x49;

const byte REG_TEMP      =  0x00;    // Register Address: Temperature Value
const byte REG_CONFIG    =  0x01;    // Register Address: Configuration


void setup()
{
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  Serial.print("Initializing SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(SS, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1) ;
  }
  Serial.println("card initialized.");
  
  // Open up the file we're going to log to!
  dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (! dataFile) {
    Serial.println("error opening datalog.txt");
    // Wait forever since we cant write data
    while (1) ;
  }


  Wire.begin();

  initT(T_a);
  initT(T_b);
  initT(T_c);
  initT(T_d);



}

void loop()
{



 float a = readC(T_a);
  float b = readC(T_b);
  float c = readC(T_c);
  float d = readC(T_d);

Serial.println(a);

  // make a string for assembling the data to log:
  String dataString = "";

  dataString+=String(millis());
  dataString+=",";

  static char outstr[15];
  dtostrf(a,7,3,outstr);
  dataString+=outstr;
  dataString+=",";

 dtostrf(b,7,3,outstr);
  dataString+=outstr;
  dataString+=",";

 dtostrf(c,7,3,outstr);
  dataString+=outstr;
  dataString+=",";

 dtostrf(d,7,3,outstr);
  dataString+=outstr;


  Serial.println(dataString);

  dataFile.println(dataString);

  // The following line will 'save' the file to the SD card after every
  // line of data - this will use more power and slow down how much data
  // you can read but it's safer! 
  // If you want to speed up the system, remove the call to flush() and it
  // will save the file only every 512 bytes - every time a sector on the 
  // SD card is filled with data.
  dataFile.flush();
  
  // Take 1 measurement every 500 milliseconds
  delay(500);
}


void initT(int sensor) {

  /* Change the resolution of the temperature measurement
       0x00 =  9 bit resolution
       0x20 = 10 bit resolution
       0x40 = 11 bit resolution
       0x60 = 12 bit resolution
  */

  Wire.beginTransmission (sensor);
  Wire.write             (REG_CONFIG);
  Wire.write             (0x00);            // Set Measurement Resolution Here
  Wire.endTransmission   ();
}

float readC(int sensor) {

byte   tempLSByte       = 0;
  byte   tempMSByte       = 0; float  floatTemperatureC = 0.0000;

Wire.beginTransmission  (sensor);
  Wire.write              (REG_TEMP);
  Wire.endTransmission    ();
  Wire.requestFrom        (sensor, 2);

  tempMSByte              = Wire.read();
  tempLSByte              = Wire.read() >> 4;
  floatTemperatureC        = tempMSByte + (tempLSByte * .0625);

  return floatTemperatureC;

}






