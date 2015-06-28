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
    Tft: Alan Senior

*/

// ---------------------------LIBRARIES------------------------ //

// Temperature sensor libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DS2438.h>
// SPI library
#include <SPI.h>
// Time libraries
#include <DS3232RTC.h>
#include <Time.h>
#include <Wire.h>
// LCD Libraries
#include <TFTv2.h>
// SD library
#include <Fat16.h>

// ------------------------END OF LIBRARIES--------------------- //

// ---------------------------Tft SETUP------------------------ //

byte lineNo = 0;
char error[20];

// Displayed messages

const char mess1[] PROGMEM = "No SD card";
const char mess2[] PROGMEM = "Error: Fat16 init";
const char mess3[] PROGMEM = "Error opening";
const char mess4[] PROGMEM = "then";
const char mess5[] PROGMEM = "Attach single sensor";
const char mess6[] PROGMEM = "only";
const char mess7[] PROGMEM = "Onewire sensor detected";
const char mess8[] PROGMEM = "Press OK to confirm";

// Setup table to refer to strings
const char* const string_table[] PROGMEM = {mess1, mess2, mess3, mess4, mess5, mess6, mess7, mess8};

// Buffer to hold text temporarily
char buffer[35];

// ----------------------END OF Tft SETUP---------------------- //

// ---------------------------SD SETUP------------------------- //

// Set select pin for SD card
#define CHIP_SELECT 4 // SD chip select pin
//Setup folder names
SdCard card;
Fat16 file;
char fileName[12] = "";

// ----------------------END OF SD SETUP----------------------- //

// -------------------------RTC SETUP------------------------- //

//Set up hour, second and minute rollover. 61 is an impossible minute
int minreg = 61;
int secreg;
char sectxt[2];
char datetime[16] = "";

// ---------------------END OF RTC SETUP---------------------- //

// -------------------------Onewire SETUP---------------------- //

// Data wire is plugged into data pin 3 on the Arduino
#define ONE_WIRE_BUS 8
//Define the max number OneWire devices
#define NumberOfDevices 5

// Setup a matrix in which to store digital sensor addresses
byte allAddress [NumberOfDevices][8];
float allAddress2 [NumberOfDevices];
// Declare variable to store number of One Wire devices
byte totalDevices;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// ---------------------END OF Onewire SETUP------------------ //


// -----------------------OVERALL SETUP----------------------- //

void setup(void) {

  Serial.begin(9600);

  // Initiate Tft
  TFT_BL_ON;
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);
  Tft.TFTinit();
  Tft.fillScreen(0, 240, 0, 320, WHITE);

  // Start I2C interface
  Wire.begin();

  // Sync the time
  setSyncProvider(RTC.get);   // the function to get the time from the RTC

  pinMode(10, OUTPUT); //Set chipselect pin to OUTPUT

  // Start OneWire interface
  sensors.begin();                             // start all digital temperature sensors
  totalDevices = discoverOneWireDevices();     // get addresses of our one wire devices into allAddress array

  for (byte i = 0; i < totalDevices; i++) {          // find out the number of total temp sensors
    sensors.setResolution(allAddress[i], 12);       // and set the a to d conversion resolution of each
  }


  // see if the card is present and can be initialized:
  if (!card.begin(CHIP_SELECT)) {
    lineNo = lineNo + 20;
    strcpy_P(buffer, (char*)pgm_read_word(&(string_table[0])));
    Tft.drawString(buffer, 5, lineNo, 2, BLACK);
  } else {

    // initialize a FAT16 volume;
    if (!Fat16::init(&card)) {
      lineNo = lineNo + 20;
      strcpy_P(buffer, (char*)pgm_read_word(&(string_table[1])));
      Tft.drawString(buffer, 5, lineNo, 2, BLACK);
    } else {

      //Setup the files and folder structure on the SD card
      //Files are named by date and time
      sprintf(fileName, "%02d%02d%02d%02d.csv", month(), day(), hour(), minute());
      // O_CREAT - create the file if it does not exist
      // O_APPEND - append if the file exists
      // O_WRITE - open for write only
      if (!file.open(fileName, O_CREAT | O_APPEND | O_WRITE)) {
        strcpy_P(buffer, (char*)pgm_read_word(&(string_table[3])));
        Tft.drawString(buffer, 5, lineNo, 2, BLACK);
      } else {

        if (file.writeError) Tft.drawString("Write error", 5, 25, 2, BLACK);
        if (!file.close()) Tft.drawString("File close error", 5, 35, 2, BLACK);

        file.open(fileName, O_CREAT | O_APPEND | O_WRITE);

        //For storing on our SD card, set up the column headers
        file.print("Time,");

        for (byte i = 0; i < totalDevices; i++) {          // find out the number of total temp sensors
          file.print("DT");
          file.print(i);
          file.print("(C)");
          file.print(",");
        }
      }
    }
  }


  // wait for everything to stabilise
  //delay(4000);

  lineNo = 10;

  Tft.fillScreen(0, 240, 0, 320, WHITE);

  printTime();

}


// --------------------END OF OVERALL SETUP------------------- //


// ---------------------------LOOP--------------------------- //

void loop() {
  //  file.open(fileName, O_CREAT | O_APPEND | O_WRITE);

  //Write the time at the top of the screen
  //Only change the time at the top of the screen if the current minute is no longer the same as the minute value of the last loop:

  if (minreg != minute()) {
    minreg = minute();
    printTime();
  }

  // Update seconds every cycle
  Tft.drawString(sectxt, 185, 10, 2, WHITE);
  sprintf(sectxt, "%02d", second());
  Tft.drawString(sectxt, 185, 10, 2, BLACK);

  secreg=second();

  //Wait for turn of second
  while(secreg == second())
  
  // Sensors request:
  sensors.requestTemperatures();                // Initiate  temperature request to all devices

  for (byte i = 0; i < totalDevices; i++) {
    char tempInfo[15] = "";
    char tempInfo2[15] = "";
    lineNo = lineNo + 20;
    float tempD = sensors.getTempC(allAddress[i]);
    char txt[10];
    dtostrf(tempD, 5, 2, txt);
    sprintf(tempInfo, "(%02d) %s C", i, txt);
    char txt2[10];
    dtostrf(allAddress2[i], 5, 2, txt2);
    sprintf(tempInfo2, "(%02d) %s C", i, txt2);
    allAddress2[i] = tempD;

    Tft.drawString(tempInfo2, 5, lineNo, 2, WHITE);
    Tft.drawString(tempInfo, 5, lineNo, 2, BLACK);

    //    file.print(tempDigi);
    //    if(i==totalDevices) {
    //      file.println(",");
    //    } else {
    //      file.print(",");
    //    }
  }
  //  file.close();
  lineNo = 10;
  delay(100);
}

// -----------------------END OF LOOP----------------------- //



// ------------------------FUNCTIONS------------------------ //

// ----------Voltmeter---------- //


// -------End of  Voltmeter------ //


// ------------ TFT -------------//

void printTime() {
  Tft.drawString(datetime, 5, 10, 2, WHITE);
  sprintf(datetime, "%02d/%02d/%02d %02d:%02d:", day(), month(), year() % 100, hour(), minute());
  Tft.drawString(datetime, 5, 10, 2, BLACK);
}

// --------- End of TFT ---------//

// ----------Onewire----------- //

byte discoverOneWireDevices() {
  byte j = 0;                                  // search for one wire devices and
  // copy to device address arrays.
  while ((j < NumberOfDevices) && (oneWire.search(allAddress[j]))) {
    j++;
  }
  for (byte i = 0; i < j; i++) {
    printAddress(allAddress[i]);                  // print address from each device address arry.
  }
  return j                      ;                 // return total number of devices found.
}

void printAddress(DeviceAddress addr) {
  byte i;
  for ( i = 0; i < 8; i++) {                      // prefix the printout with 0x
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
