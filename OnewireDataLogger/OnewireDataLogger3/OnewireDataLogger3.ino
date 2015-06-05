/*

  Title:    OnewireDataLogger
  Author:   Ben Moffatt
  Date:     02/04/2015
  Version:  0.1

  Description:
    Script to provide an all in one device to log data from a coltmeter, 
    temperature sensor, thermocouple, light sensor, pressure/ariflow sensor, 
    humidity sensor and PWM output.

  Acknowledgements:
    Onewire library: Modified by Adafruit - https://github.com/adafruit/MAX31850_OneWire
    DallasTemperature: Modified by Adafruit - https://github.com/adafruit/MAX31850_DallasTemp
    SD Card: http://arduino.cc/en/Reference/SDCardNotes
    TFT: Alan Senior

  ** CHANGED OVER TO SDFAT LIBRARY. SKETCH IS TOO LARGE**
*/

// ---------------------------LIBRARIES------------------------ //

// Temperature sensor libraries
#include <OneWire.h>
#include <DallasTemperature.h>
// SPI library
#include <SPI.h>
// Time libraries
#include <DS3232RTC.h>
#include <Time.h>
#include <Wire.h> 
// LCD Libraries
#include <Adafruit_GFX_AS.h>    // Core graphics library
#include <Adafruit_ILI9341_AS.h> // Hardware-specific library
// SD library
#include <SdFat.h>

// ------------------------END OF LIBRARIES--------------------- //

// ---------------------------TFT SETUP------------------------ //

//Define the TFT pins
#define cs   5
#define dc   6
// Invoke custom pin definitions
Adafruit_ILI9341_AS tft = Adafruit_ILI9341_AS(cs, dc);
byte lineNo = 0;

// ----------------------END OF TFT SETUP---------------------- //

// ---------------------------SD SETUP------------------------- //

// Set select pin for SD card
#define chipSelect  10 // SD chip select pin
//Setup folder names
SdFat sd;
SdFile myFile;
char fileName[12] = "";

// ----------------------END OF SD SETUP----------------------- //

// -------------------------Onewire SETUP---------------------- //

// Data wire is plugged into data pin 3 on the Arduino
#define ONE_WIRE_BUS 4
//Define the max number OneWire devices
#define NumberOfDevices 6

// Setup a matrix in which to store digital sensor addresses
byte allAddress [NumberOfDevices][6];
// Declare variable to store number of One Wire devices
byte totalDevices;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// ---------------------END OF Onewire SETUP------------------ //


// -----------------------OVERALL SETUP----------------------- //

void setup(void) {

  // Turn on TFT backlight
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  
  // Initiate TFT
  tft.init();
  tft.setRotation(0);
  
  //Set the text size
  tft.setTextSize(1);
  tft.fillScreen(ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK);
  
  // Start I2C interface
  Wire.begin();
  
  // Sync the time
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
 
   pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
 if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
  tft.drawString("NO SD CARD",5,5,4);
  delay(2000);
 }
  
  //Setup the files and folder structure on the SD card
  //Files are named by date and time
  sprintf(fileName,"%02d%02d%02d%02d%02d.csv",month(),day(), hour(), minute());
  
  if (!myFile.open(fileName, O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("opening test.txt for write failed");
  }
 
  myFile.open(fileName, O_CREAT | O_APPEND | O_WRITE);
  
  //For storing on our SD card, set up the column headers
  myFile.print("Time,");
 
  // Start OneWire interface
  sensors.begin();	                                           // start all digital temperature sensors
  totalDevices = discoverOneWireDevices();     // get addresses of our one wire devices into allAddress array 

  for (byte i=0; i < totalDevices; i++) {            // find out the number of total temp sensors
  sensors.setResolution(allAddress[i], 12);       // and set the a to d conversion resolution of each
  myFile.print("DT");
  myFile.print(i);
  myFile.print("(C)");
  myFile.print(",");
  Serial.println(i);
  }
  
  myFile.println();
  
  myFile.close();

  // wait for everything to stabilise
  delay(1500);
  
  tft.fillScreen(ILI9341_WHITE);
}


// --------------------END OF OVERALL SETUP------------------- //


// ---------------------------LOOP--------------------------- //

void loop() {
  myFile.open(fileName, O_CREAT | O_APPEND | O_WRITE);

  // make a string for assembling the data to log:
//  String dataString = "";
  
  // Sensors request:
  sensors.requestTemperatures();                // Initiate  temperature request to all devices

  for (byte i=0; i < totalDevices; i++) {
    char tempInfo[12] = "";
    lineNo = lineNo + 10;
    float tempDigi = sensors.getTempC(allAddress[i]);
    sprintf(tempInfo,"(%02d) %02d C",i,tempDigi);
    
    tft.drawString(tempInfo,5,lineNo,2);
    
    myFile.print(tempDigi);
    if(i==totalDevices) {
      myFile.println(",");
    } else {
      myFile.print(",");
    }
  }
  myFile.close();
  lineNo = 0;
}

// -----------------------END OF LOOP----------------------- //



// ------------------------FUNCTIONS------------------------ //

// ----------Sensor Setup---------- //

void sensorSetup(void) {
  
}

// -------End of Sensor Setup------ //

// --------Prev Sensor Setup------- //

void prevSensorInit(void) {
  
}

// ----End of Prev Sensor Setup---- //

// ----------Voltmeter---------- //


// -------End of  Voltmeter------ //

// ------------TMP37------------- //


// --------End of TMP36------- //


// ----------Onewire----------- //

byte discoverOneWireDevices() {
  byte j=0;                                    // search for one wire devices and
                                                   // copy to device address arrays.
  while ((j < NumberOfDevices) && (oneWire.search(allAddress[j]))) {        
    j++;
  }
  for (byte i=0; i < j; i++) {                      
    printAddress(allAddress[i]);                  // print address from each device address arry.
  }
  return j                      ;                 // return total number of devices found.
}

void printAddress(DeviceAddress addr) {
  byte i;
  for( i=0; i < 8; i++) {                         // prefix the printout with 0x
      if (addr[i] < 16) {
      }
      if (i < 7) {
      }
    }
}

void printTemperature(DeviceAddress addr) {
  float tempC = sensors.getTempC(addr);           // read the device at addr.
  if (tempC == -127.00) {
  } else {
  }
}

// --------End of Onewire------- //


// ---------------------END OF FUNCTIONS--------------------- //
