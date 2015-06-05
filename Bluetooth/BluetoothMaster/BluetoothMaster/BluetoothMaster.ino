/*

  Title:    Serial monitoring of bluetooth
  Author:   Ben Moffatt
  Date:     17/03/2015
  Version:  0.1

  Description:
    Monitor bluetooth and output the readings on the display
    
*/

// ---------------------------LIBRARIES------------------------ //

// LCD Library
#include <ShiftRegLCD123.h>

// ------------------------END OF LIBRARIES--------------------- //

// -------------------------LCD SETUP------------------------- //


const byte dataPin  = 2;    // SR Data from Arduino pin 10
const byte clockPin = 3;    // SR Clock from Arduino pin 11
const byte enablePin = 4;   // LCD enable from Arduino pin 12

// Instantiate an LCD object using ShiftRegLCD123 wiring.
// LCD enable directly from Arduino.
ShiftRegLCD123 srlcd(dataPin, clockPin, enablePin, SRLCD123);

// ---------------------END OF LCD SETUP---------------------- //

void setup(void) {
  Serial.begin(38400);		                                   // set up for error checking
  
    // initialize LCD and set display size
  // LCD size 20 columns x 2 lines, small (normal) font
  srlcd.begin(20,2);
  srlcd.setCursor(0,1);
  srlcd.print("Bluetooth monitoring");
  
  delay(3000);
  
}

void loop() {
  
  // when characters arrive over the serial port...
  if (Serial.available()) {
    // wait a bit for the entire message to arrive
    delay(100);
    // clear the screen
    srlcd.clear();
    // read all the available characters
    while (Serial.available() > 0) {
      // display each character to the LCD
      srlcd.write(Serial.read());
    }
  }
}
