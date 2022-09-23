/* Take 1: 
 *  Aim: To set up sensor with TFT Screen
 *  
 *  Measure pressure and diplay on the screen
 *  Set "Set Pressure" with a button
 *  
 *  Show alert when 
 *  
 */

 // ------------------------------------Step I : Include Libraries-------------------------
  #include <Wire.h>                   // For Pr sensor
  #include "Adafruit_MPRLS.h"         // This is the MPRLS library required for the pressure sensor 
  #include <TFT.h>  
  #include <SPI.h> // For Screen - TFT
 
 // ------------------------------------Step II : Pin definitions & Instances -------------------------
  #define RESET_PIN  -1  // set to any GPIO pin # to hard-reset on begin()
  #define EOC_PIN    -1 
  Adafruit_MPRLS mpr = Adafruit_MPRLS(RESET_PIN, EOC_PIN); 
  
  #define cs   10
  #define dc   9
  #define rst  8
  TFT TFTscreen = TFT(cs, dc, rst);
  
  
  
 // ------------------------------------Step III : Global Variables -------------------------
  char sensorPrintout[10];
  char setPressure_str[10];
  const int setButtonPin = 3; // "Set" button pin on pin 3  
  const int sounderButtonPin = 2; // Sounder On/Off button
  float setPressure = 0; // To store set pressure
  float setPressure_low; // To store low pressure 
  float setPressure_high; // To store low pressure 
  float tol = 2; // % tolerance for alert, should be 20
  
 // ------------------------------------Step IV : Write void setup() -------------------------
 void setup() {
  Serial.begin(115200);

  // For MPRLS
  mpr.begin();
  
  //For Screen
  TFTscreen.begin(); 
  TFTscreen.background(0, 0, 0); // clear the screen with a black background
  TFTscreen.setTextSize(2); //set the text size

}

 // ------------------------------------Step V : Write void loop() -------------------------

void loop() {

  int setButtonState;
  setButtonState = digitalRead(setButtonPin);

  int sounderButtonState;
  sounderButtonState = digitalRead(sounderButtonPin);
  
  // set a random font color
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.background(0, 0, 0); // Background colour = black
  
  float pressure_hPa = mpr.readPressure(); // read pressure
  dtostrf(pressure_hPa,8,2,sensorPrintout); // convert float to string
  TFTscreen.text(sensorPrintout, 6, 57); // print pressure output
  delay(250);

  if(setButtonState == HIGH) // Means that button is pressed
  {
    TFTscreen.background(0,0,0);
    TFTscreen.text("Setting pressure ...",6,57);
    setPressure = mpr.readPressure(); // Set current pressure as "Set" pressure
    dtostrf(setPressure,8,2,setPressure_str); // convert float to string
    dtostrf(pressure_hPa,8,2,sensorPrintout); // convert float to string
    TFTscreen.background(0,0,0);
    TFTscreen.text("Pressure set at:",6,57);
    TFTscreen.text(sensorPrintout,6,77);
    delay(5000);
    setPressure_low = (1-(tol/100))*setPressure;
    setPressure_high = (1+(tol/100))*setPressure;
  }
  else{

  if (setPressure == 0)
  {  
    TFTscreen.text("Pressure not set",6,77);
    delay(2000);
  }

  else if (pressure_hPa <= (setPressure_low))// changed from "setPressure_low" to "setPressure" for testing
  {
    if (sounderButtonState == 1) // Meaning that the sounder button is On
    {
          tone(6, 31, 1000/4); // Alarm note
    }
    TFTscreen.background(255,255,255);
    TFTscreen.stroke(0, 0, 255); // Change colour to red
    TFTscreen.text("Low Pressure Alert!",6,12);
    TFTscreen.text("Now:",6,32);
    TFTscreen.text(sensorPrintout,6,52);
    TFTscreen.text("Set:",6,72);
    TFTscreen.text(setPressure_str,6,92);
    delay(5000);
    
  }
//  else if (pressure_hPa >= setPressure) // changed from "setPressure_high" to "setPressure" for testing
//  {
//    TFTscreen.background(0,0,0);
//    TFTscreen.text("High Pressure Alert!",6,12);
//    TFTscreen.text("Now:",6,32);
//    TFTscreen.text(sensorPrintout,6,52);
//    TFTscreen.text("Set:",6,72);
//    TFTscreen.text(setPressure_str,6,92);
//    delay (5000);
//  }
  else 
  {
    TFTscreen.background(0,0,0);
    TFTscreen.stroke(0, 255,0); // Change colour to green
    TFTscreen.text("Pressure OK",6,12);
    TFTscreen.text("Now:",6,32);
    TFTscreen.text(sensorPrintout,6,52);
    TFTscreen.text("Set:",6,72);
    TFTscreen.text(setPressure_str,6,92);
    delay(5000);
  }
  
  }

 
}
