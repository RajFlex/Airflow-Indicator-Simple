/*
 * Aim: To compare ABP2 and MPRLS sensor readings
 * Example code for ABP2 from the datasheet https://www.farnell.com/datasheets/3217178.pdf
 */
 
// ------------------------------------Step I : Include Libraries-------------------------
#include<Arduino.h>
#include<SPI.h>
#include <Wire.h>                   // For Pr sensor
#include "Adafruit_MPRLS.h"         // This is the MPRLS library required for the pressure sensor 
#include <TFT.h>                    // For TFT Screen
// ------------------------------------Step II : Pin definitions & Instances -------------------------
  #define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
  #define EOC_PIN    -1 
  Adafruit_MPRLS mpr = Adafruit_MPRLS(RESET_PIN, EOC_PIN); 
  
  #define cs   9//4
  #define dc   8//9
  #define rst  7//8
  TFT TFTscreen = TFT(cs, dc, rst);
// ------------------------------------Step III : Global Variables -------------------------
double press_counts = 0; // digital pressure reading [counts]
double temp_counts = 0; // digital temperature reading [counts]
double pressure = 0; // pressure reading [bar, psi, kPa, etc.]
double pressure_Pa_ABP = 0; // Same thing but in hPa
double temperature = 0; // temperature reading in deg C
double outputmax = 15099494; // output at maximum pressure [counts]
double outputmin = 1677722; // output at minimum pressure [counts]
double pmax = 1; // maximum value of pressure range [bar, psi, kPa, etc.]
double pmin = 0; // minimum value of pressure range [bar, psi, kPa, etc.]
double percentage = 0; // holds percentage of full scale data
char printBuffer[200], cBuff[20], percBuff[20], pBuff[20], tBuff[20];

// For MPRLS
  char sensorPrintout[10];
  char setPressure_str[10];
  char differenceBuff[20];
  
 // ------------------------------------Step IV : Write void setup() -------------------------
void setup() {
 Serial.begin(9600);
// For 
 while (!Serial) {
 delay(10);
 }
 sprintf(printBuffer, "\nStatus Register, 24-bit Sensor data, Digital Pressure Counts,\
 Percentage of full scale pressure,Pressure Output, Temperature\n");
 Serial.println(printBuffer);
 SPI.begin();
 pinMode(10, OUTPUT); // pin 10 as SS
 digitalWrite(10, HIGH); // set SS High

  // For MPRLS
  mpr.begin();
  
  //For Screen
  TFTscreen.begin(); 
  TFTscreen.background(0, 0, 0); // clear the screen with a black background
  TFTscreen.setTextSize(1); //set the text size
    TFTscreen.stroke(255, 255, 255); // set a font color
    TFTscreen.text("TFT Screen is working", 6, 57); // print 
}

// ------------------------------------Step V : Write void loop() -------------------------
void loop() {
 delay(1);
 while (1) {
 uint8_t data[7] = {0xFA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // holds output data
 uint8_t cmd[3] = {0xAA, 0x00, 0x00}; // command to be sent
 SPI.beginTransaction(SPISettings(200000, MSBFIRST, SPI_MODE0)); //SPI at 200kHz
 digitalWrite(10, LOW); // set SS Low
 SPI.transfer(cmd, 3); // send Read Command
 digitalWrite(10, HIGH); // set SS High
 delay(10); // wait for conversion
 digitalWrite(10, LOW);
 SPI.transfer(data, 7);
 digitalWrite(10, HIGH);
 SPI.endTransaction();
 press_counts = data[3] + data[2] * 256 + data[1] * 65536; // calculate digital pressure counts
 temp_counts = data[6] + data[5] * 256 + data[4] * 65536; // calculate digital temperature counts
 temperature = (temp_counts * 200 / 16777215) - 50; // calculate temperature in deg c
 percentage = (press_counts / 16777215) * 100; // calculate pressure as percentage of full scale
 //calculation of pressure value according to equation 2 of datasheet
 pressure = ((press_counts - outputmin) * (pmax - pmin)) / (outputmax - outputmin) + pmin;
 pressure_Pa_ABP = pressure*68.9475729318 * 100; // Convert psi to Pa 
 dtostrf(press_counts, 4, 1, cBuff);
 dtostrf(percentage, 4, 3, percBuff);
 dtostrf(pressure_Pa_ABP, 4, 3, pBuff);
 dtostrf(temperature, 4, 3, tBuff);
 /*
 The below code prints the raw data as well as the processed data
 Data format : Status Register, 24-bit Sensor Data, Digital Counts, percentage of full scale
pressure, pressure output,
 temperature
 */
// sprintf(printBuffer, "%x\t%2x %2x %2x\t%s\t%s\t%s\t%s \n", data[0], data[1], data[2], data[3],
// cBuff, percBuff, pBuff, tBuff);
// Serial.print(printBuffer);
 delay(10);

 // For TFT screen 
  TFTscreen.stroke(255, 255, 255); // set a font color
  TFTscreen.background(0, 0, 0); // Background colour = black
  
  float pressure_hPa_MPRLS = mpr.readPressure(); // read pressure
  float pressure_Pa_MPRLS = pressure_hPa_MPRLS * 100; // Convert hPa to Pa  
  dtostrf(pressure_Pa_MPRLS,4,2,sensorPrintout); // convert float to string
  TFTscreen.text("MPRLS (Pa):", 6, 17);
  TFTscreen.text(sensorPrintout, 6, 27); // print pressure output from MPRLS
  Serial.println(sensorPrintout);
  
  TFTscreen.text("ABP2 (Pa):", 6, 47);
  TFTscreen.text(pBuff, 6, 57); // print pressure output from ABP2
  
  // Calculate difference
  double difference = pressure_Pa_MPRLS - pressure_Pa_ABP;
  dtostrf(difference,4,2,differenceBuff); // convert float to string
  TFTscreen.text("Difference (Pa):", 6, 77);
  TFTscreen.text(differenceBuff, 6, 87); // print difference

  delay(250);
 }

  
}
