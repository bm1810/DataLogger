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

*/

// ---------------------------LIBRARIES------------------------ //

// Temperature sensor libraries
#include <OneWire.h>
#include <DallasTemperature.h>
// SPI library
#include <SPI.h>
// LCD Libraries
#include <TFTv2.h>
// SD library
#include <SD.h>

// ------------------------END OF LIBRARIES--------------------- //

// Some often used words
String yes = "Yes";
String no = "No";
String ok = "OK";
String cancel = "Cancel";
String check = "[*]";
String nocheck = "[ ]";

// Define button pins
#define buttonPin1 2
#define buttonPin2 4
#define buttonPin3 7

// -------------------------Onewire SETUP---------------------- //

// Data wire is plugged into data pin 3 on the Arduino
#define ONE_WIRE_BUS 2
//Define the max number of DS18b20s
#define NumberOfDevices 6

// Setup a matrix in which to store digital sensor addresses
byte allAddress [NumberOfDevices][6];
// Declare variable to store number of One Wire devices
byte totalDevices;

// ---------------------END OF Onewire SETUP------------------ //

// -------------------------LCD SETUP------------------------- //

// ---------------------END OF LCD SETUP---------------------- //


// -----------------------OVERALL SETUP----------------------- //

void setup(void) {
  Serial.begin(9600);		                                   // set up for error checking
    
    TFT_BL_ON;                                          //turn on the background light 
    
    Tft.TFTinit();                                      //init TFT library 
  
  // Setup a oneWire instance to communicate with any OneWire devices
  OneWire oneWire(ONE_WIRE_BUS);
  // Pass our oneWire reference to Dallas Temperature. 
  DallasTemperature sensors(&oneWire);

  sensors.begin();	                                           // start all onewire sensors
  totalDevices = discoverOneWireDevices();     // get addresses of our one wire devices into allAddress array 

  // wait for everything to stabilise
  delay(1500);
  
}


// --------------------END OF OVERALL SETUP------------------- //


// ---------------------------LOOP--------------------------- //

void loop() {
  

  // make a string for assembling the data to log:
  String dataString = "";
  
  // Sensors request:
  sensors.requestTemperatures();                // Initiate  temperature request to all devices
  
  csvFile = SD.open(folderName, FILE_WRITE);
  csvFile.println(dataString);
  csvFile.close();
  
  Serial.println(dataString);


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
