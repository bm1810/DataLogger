/*

  Title:    Temperature and Bluetooth Output Script
  Author:   Ben Moffatt
  Date:     21/02/2015
  Version:  0.1

  Description:
    Attempt to output the temperature from a DS18B20 temperature sensor to serial monitor on a windows PC.

  Acknowledgements:
    Digital temperature sensor: John Lowen http://www.vwlowen.co.uk/arduino/ds18b20/ds18b20.htm
  
*/

// LCD libraries
#include <stdint.h>
#include <TFTv2.h>
#include <SPI.h>

// Temperature sensor libraries
#include <OneWire.h>
#include <DallasTemperature.h>

float tempDigiprev = 0;

// -------------------------DS18B20 SETUP---------------------- //

// Data wire is plugged into data pin 3 on the Arduino
#define ONE_WIRE_BUS 2
//Define the max number of DS18b20s
#define NumberOfDevices 8

// Setup a matrix in which to store digital sensor addresses
byte allAddress [NumberOfDevices][8];
// Declare variable to store number of One Wire devices
byte totalDevices;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// ---------------------END OF DS18B20 SETUP------------------ //

// -----------------------OVERALL SETUP----------------------- //

void setup(void) {
  Serial.begin(38400);		                                   // set up for error checking	

  TFT_BL_ON;      // turn on the background light
  Tft.TFTinit();  // init TFT library

  Tft.drawString("Temperature Sensor", 10, 10, 2, WHITE);
  
  //For storing on our SD card, set up the column headers
  String dataString = "";
  Serial.print("Setup starting"); 
  sensors.begin();	                                           // start all digital temperature sensors
  totalDevices = discoverOneWireDevices();     // get addresses of our one wire devices into allAddress array 

  for (byte i=0; i < totalDevices; i++) {            // find out the number of total temp sensors
  sensors.setResolution(allAddress[i], 12);       // and set the a to d conversion resolution of each
  dataString += "DT";
  dataString += String(i);
  dataString += "(C)";
  dataString += ",";
  }

  Serial.print(dataString);

  // wait for everything to stabilise
  delay(1500);


}


// --------------------END OF OVERALL SETUP------------------- //

// ---------------------------LOOP--------------------------- //

void loop() {

  
  // Sensors request:
  sensors.requestTemperatures();                // Initiate  temperature request to all devices

  // Print the temperature sensor output
  for (byte i=0; i < totalDevices; i++) {
    float tempDigi = sensors.getTempC(allAddress[i]);
    Tft.drawFloat(tempDigiprev, 10, 30, 2, BLACK);
    Tft.drawFloat(tempDigi, 10, 30, 2, WHITE);
    Serial.print(tempDigi);
    tempDigiprev = tempDigi;
  }


}

// -----------------------END OF LOOP----------------------- //

// ------------------------FUNCTIONS------------------------ //

// ----------Onewire----------- //

byte discoverOneWireDevices() {
  byte j=0;                                    // search for one wire devices and
                                                   // copy to device address arrays.
  while ((j < NumberOfDevices) && (oneWire.search(allAddress[j]))) {        
    j++;
  }
  for (byte i=0; i < j; i++) {
    //Serial.print("Device ");
    //Serial.print(i);  
    //Serial.print(": ");                          
    printAddress(allAddress[i]);                  // print address from each device address arry.
  }
  //Serial.print("\r\n");
  return j                      ;                 // return total number of devices found.
}

void printAddress(DeviceAddress addr) {
  byte i;
  for( i=0; i < 8; i++) {                         // prefix the printout with 0x
      //Serial.print("0x");
      if (addr[i] < 16) {
        //Serial.print('0');                        // add a leading '0' if required.
      }
      //Serial.print(addr[i], HEX);                 // print the actual value in HEX
      if (i < 7) {
        //Serial.print(", ");
      }
    }
  //Serial.print("\r\n");
}

void printTemperature(DeviceAddress addr) {
  float tempC = sensors.getTempC(addr);           // read the device at addr.
  if (tempC == -127.00) {
    //Serial.print("Error getting temperature.");
  } else {
    //Serial.print(tempC);                          // and print its value.
    //Serial.print(" C (");
  }
}

// --------End of Onewire------- //


// ---------------------END OF FUNCTIONS--------------------- //
