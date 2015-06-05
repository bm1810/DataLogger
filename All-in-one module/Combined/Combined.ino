/*

  Title:    Temperature, Voltage, LCD, RTC, SD card and Bluetooth Combined Script
  Author:   Ben Moffatt
  Date:     25/01/2015
  Version:  0.1

  Description:
    Combination of all previous scripts to record temperature and voltages
    simultaneously, stored with a real time and date. The data is recorded to the
    onboard SD card and transmitted over bluetooth.

  Acknowledgements:
    Digital temperature sensor: John Lowen http://www.vwlowen.co.uk/arduino/ds18b20/ds18b20.htm
    Thermocouple: http://playground.arduino.cc/Main/MAX6675Library
    SD Card: http://arduino.cc/en/Reference/SDCardNotes
    DS3231 RTC: https://github.com/JChristensen/DS3232RTC

*/

// ---------------------------LIBRARIES------------------------ //

// Temperature sensor libraries
#include <OneWire.h>
#include <DallasTemperature.h>
// RTC libraries
#include <DS3232RTC.h> 
#include <Time.h>
#include <Wire.h>  
// LCD Library
#include <PCD8544.h>
// MAX6675 Library (thermocouple)
#include <max6675.h>
//SD card and SPI libraries
#include <SD.h>
#include <SPI.h>

// ------------------------END OF LIBRARIES--------------------- //

// -------------------------DS18B20 SETUP---------------------- //

// Data wire is plugged into data pin 3 on the Arduino
#define ONE_WIRE_BUS 2
//Define the max number of DS18b20s
#define NumberOfDevices 2

// Setup a matrix in which to store digital sensor addresses
byte allAddress [NumberOfDevices][8];
// Declare variable to store number of One Wire devices
byte totalDevices;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// ---------------------END OF DS18B20 SETUP------------------ //

// -------------------------RTC SETUP------------------------- //

//Setup millisec value
volatile unsigned long millistart;
//Set up minute rollover. 61 is an impossible minute
byte minreg = 60;

// ---------------------END OF RTC SETUP---------------------- //

// -------------------------LCD SETUP------------------------- //

static PCD8544 lcd;
byte lineNo = 0;

// ---------------------END OF LCD SETUP---------------------- //

// -----------------------MAX6675 SETUP---------------------- //

//Set up the pins that the MAX6675 connects to
int thermoDO = 8;
int thermoCS = 9;
int thermoCLK = A0;

// Let library know which pins the thermocouple is conected to:
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

//Define what the thermocouple temperature will be
int tctemp = 0;

// --------------------END OF MAX6675 SETUP------------------ //

const int chipSelect = 10;

// ----------------------VOLTMETER SETUP--------------------- //

// number of analog samples to take per reading
#define NUM_SAMPLES 10
// reference voltage for the Arduino - MEASURE FOR ACCURACY
#define vPow 5.0
// resistance values - MEASURE FOR ACCURACY
#define r1 100000
#define r2 10000

// ------------------END OF VOLTMETER SETUP------------------ //

// -----------------------SD CARD SETUP---------------------- //

char folderName[18] = "";
File csvFile;

// -------------------END OF SD CARD SETUP------------------- //

// -----------------------OVERALL SETUP----------------------- //

void setup(void) {
//  Serial.begin(9600);		                                   // set up for error checking
  Wire.begin();				                                   // start the I2C interface

  lcd.begin(84, 48);		                                   // setup the LCD resolution
  lcd.setCursor(0, lineNo);                               // show setup text on LCD
  lcd.print("Starting up...");
  lineNo++;
//  lcd.setCursor(0, lineNo);                               // put cursor in next line
//  lcd.print("Discovered:");                                // list all connected sensors

  //For storing on our SD card, set up the column headers
  String dataString = "";
 
  dataString += "Time";
  dataString += ",";
 
  sensors.begin();	                                           // start all digital temperature sensors
  totalDevices = discoverOneWireDevices();     // get addresses of our one wire devices into allAddress array 
//  if(totalDevices > 0) { lineNo++; };                  // add a line if there are digital temperature sensors connected
//  lcd.setCursor(0, lineNo);
//  lcd.print(totalDevices);
//  lcd.print(" x DS18b20");
  for (byte i=0; i < totalDevices; i++) {            // find out the number of total temp sensors
  sensors.setResolution(allAddress[i], 12);       // and set the a to d conversion resolution of each
  dataString += "Digital Temp ";
  dataString += String(i);
  dataString += ",";
  }
/*
  if(thermocouple.readCelsius() != NAN) {
    lineNo++;
    lcd.setCursor(0, lineNo);                              // put cursor in next line
    lcd.print("Thermocouple");                           // thermocouple must be connected
    dataString += "Thermocouple";
    dataString += ",";
    sensorCount++;
  }

  if(analogRead(2) > 0) {
	lineNo++;
    lcd.setCursor(0, lineNo);                              // put cursor in next line
    lcd.print("Voltmeter");                                  // voltmeter must be connected
    dataString += "Voltmeter";
    dataString += ",";
    sensorCount++;
  }

  if(analogRead(4) > 0) {
    lineNo++;
    lcd.setCursor(0, lineNo);                              // put cursor in next line
    lcd.print("Analog Temp");                            // TMP36 must be connected
    dataString += "Analog Temp";
    dataString += ",";
    sensorCount++;
  }

  delay(2000);

  // If there are no sensors connected, tell the user to connect some!
  if(sensorCount=0) {
  lcd.print("No sensors detected. Reset to try again");
  return;
  }
*/
  //Set up the RTC

  lcd.clear();                                                    // clear the LCD ready for the next menu
  lineNo = 0;

  //Set the status of the interrupt pin
  pinMode(A3, INPUT);
  //Include an interrupt from the clock
  attachInterrupt(A3, SecIntrpt, RISING);
  //RTC.setAlarm(ALM1_EVERY_SECOND);
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if(timeStatus() != timeSet) {
      lcd.print("Unable to sync with the RTC");
  } else {
      lcd.print("System time   set"); 
  }

  // wait for everything to stabilise
  delay(1500);

  //Setup the SD card
  lcd.print("Initializing ");
  lineNo++;
  lcd.setCursor(0, lineNo);
  lcd.print("SD card...");
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(10, OUTPUT);

  lcd.clear();                                                    // clear the LCD ready for the next menu
  lineNo = 0;

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    lcd.clear();
    lineNo = 0;
    lcd.setCursor(0,lineNo);
    lcd.println("SD Card failedor not present");
    // don't do anything more:
    return;
  }
  
  lineNo = 0;
  lcd.setCursor(0,lineNo);
  lcd.clear();
  lcd.print("SD card");
  lineNo++;
  lcd.setCursor(0,lineNo);
  lcd.print("initialized.");

  lcd.clear();                                                    // clear the LCD ready for the next menu
  lineNo = 0;

  //Setup the files and folder structure on the SD card
  //Folders are named by date
  //char* folderName; 
  sprintf(folderName,"%02d%02d%02d",year(),month(),day());
  Serial.println(folderName);
  if (SD.exists(folderName)) {
    //append on the filename to the buffer
    sprintf(folderName,"%c%02d%02d%02d/%02d%02d.csv",47,year(),month(),day(),hour(),minute());
    lcd.setCursor(0,lineNo);
    lcd.print("Creating");
    lcd.print(folderName);
    //Serial.print(folderName);
    csvFile = SD.open(folderName, FILE_WRITE);
    csvFile.println(dataString);
    csvFile.close();
  } else {
    SD.mkdir(folderName);
    //append on the filename to the buffer
    sprintf(folderName,"%c%02d%02d%02d/%02d%02d.csv",47,year(),month(),day(),hour(),minute());
    lcd.setCursor(0,lineNo);
    lcd.print("Creating ");
    lcd.print(folderName);
    //Serial.print(folderName);
    csvFile = SD.open(folderName, FILE_WRITE);
    csvFile.println(dataString);
    csvFile.close();
  }
  
  lcd.clear();                                                    // clear the LCD ready for the next menu
  lineNo = 0;
}


// --------------------END OF OVERALL SETUP------------------- //


// ---------------------------LOOP--------------------------- //

void loop() {

  // make a string for assembling the data to log:
  String dataString = "";
  
  //Only change the time at the top of the screen if the current minute is no longer the same as the minute value of the last loop:
  if(minreg != minute()) {
    minreg = minute();
    lcd.setCursor(0,lineNo);
    char datetime[16] = "";
    sprintf(datetime, "%02d/%02d/%02d %02d:%02d", day(), month(), year()%100, hour(), minute());
    lcd.print(datetime);
    dataString += datetime;
   }

  dataString += ",";
    
  // Sensors request:
  sensors.requestTemperatures();                // Initiate  temperature request to all devices

  // Print the temperature sensor output
  for (byte i=0; i < totalDevices; i++) {
    lineNo++;
    lcd.setCursor(0,lineNo);
    float tempDigi = sensors.getTempC(allAddress[i]);
    lcd.print(" DT");
    lcd.print(i);
    lcd.print(": ");
    lcd.print(tempDigi);
    lcd.print("C");
    
    dataString += tempDigi;
    dataString += ",";
  }


  //Thermocouple display
  float thermoc = thermocouple.readCelsius();
  lcd.setCursor(0,3);
  lcd.print(" TC1: ");
  lcd.print(thermoc);
  dataString += thermoc;
  dataString += ",";
  

  //Voltage display
 
  float calcVolt = findVoltage(vPow,r1,r2); 
  lcd.setCursor(0,4);

  lcd.print(" VM:  ");
  lcd.print(calcVolt);
  lcd.print(" V");
  dataString += calcVolt;
  dataString += ",";

  //Analog temperature sensor

  float anlTemp = analogueTemp(); 
  lcd.setCursor(0,5);
  lcd.print(" AT:  ");
  lcd.print(anlTemp);
  lcd.print("C");
  dataString += anlTemp;
  dataString += ",";
  
  csvFile = SD.open(folderName, FILE_WRITE);
  csvFile.println(dataString);
  csvFile.close();
  delay(10);

}

// -----------------------END OF LOOP----------------------- //



// ------------------------FUNCTIONS------------------------ //

// ----------Voltmeter---------- //

float findVoltage(float vPowa, float r1a, float r2a) {

 // calculate the real voltage
 float anavoltage = (analogRead(A2) * vPowa) / 1024.0;
 float voltage = anavoltage / ((float)r2a / ((float)r1a + (float)r2a));

 return voltage;
}


// -------End of  Voltmeter------ //

// ------------TMP37------------- //

float analogueTemp()
{
  // read the input on analog pin 0 (TMP36 input):
  int anlsensorValue = analogRead(A1);
  
  // Convert the input signal into a voltage
  float voltage = anlsensorValue * 5.0;
  voltage /= 1024.0;
  
  // convert the sensor value to temp in C
  float anlTemp = (voltage - 0.5)*100;
  return anlTemp;
}

// --------End of TMP36------- //

// ------------RTC------------ //

// Setup the RTC alarm


// Second counting interrupt
void SecIntrpt() {

  //Define the chip value of milliseconds at the start of the count
  millistart = millis();

}


// ---------End of  RTC--------- //


// ----------Onewire----------- //

byte discoverOneWireDevices() {
  byte j=0;                                    // search for one wire devices and
                                                   // copy to device address arrays.
  while ((j < NumberOfDevices) && (oneWire.search(allAddress[j]))) {        
    j++;
  }
  for (byte i=0; i < j; i++) {
    Serial.print("Device ");
    Serial.print(i);  
    Serial.print(": ");                          
    printAddress(allAddress[i]);                  // print address from each device address arry.
  }
  Serial.print("\r\n");
  return j                      ;                 // return total number of devices found.
}

void printAddress(DeviceAddress addr) {
  byte i;
  for( i=0; i < 8; i++) {                         // prefix the printout with 0x
      Serial.print("0x");
      if (addr[i] < 16) {
        Serial.print('0');                        // add a leading '0' if required.
      }
      Serial.print(addr[i], HEX);                 // print the actual value in HEX
      if (i < 7) {
        Serial.print(", ");
      }
    }
  Serial.print("\r\n");
}

void printTemperature(DeviceAddress addr) {
  float tempC = sensors.getTempC(addr);           // read the device at addr.
  if (tempC == -127.00) {
    Serial.print("Error getting temperature.");
  } else {
    Serial.print(tempC);                          // and print its value.
    Serial.print(" C (");
  }
}

// --------End of Onewire------- //


// ---------------------END OF FUNCTIONS--------------------- //
