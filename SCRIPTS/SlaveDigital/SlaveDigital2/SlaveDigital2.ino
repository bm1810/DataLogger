/*

  Title:    Temperature, Voltage, LCD, RTC, SD card and Bluetooth Combined Script
  Author:   Ben Moffatt
  Date:     14/02/2015
  Version:  0.3

  Description:
    Combination of all previous scripts to record temperature and voltages
    simultaneously, stored with a real time and date. The data is recorded to the
    onboard SD card and transmitted over bluetooth.

  Acknowledgements:
    Digital temperature sensor: John Lowen http://www.vwlowen.co.uk/arduino/ds18b20/ds18b20.htm
    Thermocouple: http://playground.arduino.cc/Main/MAX6675Library
    SD Card: http://arduino.cc/en/Reference/SDCardNotes
    DS3231 RTC: https://github.com/JChristensen/DS3232RTC
  
  // This version of the script has been stripped down as much as possible to allow for the size limitations of the Arduino chip
  

*/

// ---------------------------LIBRARIES------------------------ //

// Temperature sensor libraries
#include <OneWire.h>
#include <DallasTemperature.h>
// LCD Library
#include <ShiftRegLCD123.h>
// MAX6675 Library (thermocouple)
#include <max6675.h>

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

// -------------------------DS18B20 SETUP---------------------- //

// Data wire is plugged into data pin 3 on the Arduino
#define ONE_WIRE_BUS 2
//Define the max number of DS18b20s
#define NumberOfDevices 8

// Setup a matrix in which to store digital sensor addresses
byte allAddress [NumberOfDevices][8];
// Declare variable to store number of One Wire devices
byte totalDevices;



// ---------------------END OF DS18B20 SETUP------------------ //

// -------------------------LCD SETUP------------------------- //


const byte dataPin  = 10;    // SR Data from Arduino pin 10
const byte clockPin = 11;    // SR Clock from Arduino pin 11
const byte enablePin = 12;   // LCD enable from Arduino pin 12

// Instantiate an LCD object using ShiftRegLCD123 wiring.
// LCD enable directly from Arduino.
ShiftRegLCD123 srlcd(dataPin, clockPin, enablePin, SRLCD123);

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

// ----------------------VOLTMETER SETUP--------------------- //

// number of analog samples to take per reading
//#define NUM_SAMPLES 10
// reference voltage for the Arduino - MEASURE FOR ACCURACY
#define vPow 5.0
// resistance values - MEASURE FOR ACCURACY
#define r1 100000
#define r2 10000

// ------------------END OF VOLTMETER SETUP------------------ //


// -----------------------OVERALL SETUP----------------------- //

void setup(void) {
  Serial.begin(9600);		                                   // set up for error checking
    
  // Setup a oneWire instance to communicate with any OneWire devices
  OneWire oneWire(ONE_WIRE_BUS);
  // Pass our oneWire reference to Dallas Temperature. 
  DallasTemperature sensors(&oneWire);

  sensors.begin();	                                           // start all onewire sensors
  totalDevices = discoverOneWireDevices();     // get addresses of our one wire devices into allAddress array 
  
  int choice = 0;
  
  //Set up button pin modes
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
  
  // initialize LCD and set display size
  // LCD size 20 columns x 2 lines, small (normal) font
  srlcd.begin(20,2);
  srlcd.setCursor(0,1);
  srlcd.print("New");
  srlcd.setCursor(0,2);
  srlcd.print("setup?"); 
  srlcd.setCursor(9,1);
  srlcd.print(nocheck);
  srlcd.print(yes);
  srlcd.setCursor(10,1);
  srlcd.print(nocheck);
  srlcd.print(no);
  
  while(digitalRead(buttonPin1)==0 or digitalRead(buttonPin2)==0) {}
  if(buttonPin1==1) {
    choice=0;
    srlcd.setCursor(9,1);
    srlcd.print(check);
  }
  if(buttonPin2==1) {
    choice=1;
    srlcd.setCursor(10,1);
    srlcd.print(nocheck);
  }
  
  while(buttonPin3==0){}
  if(choice=0){
    sensorSetup();
  } else {
    prevSensorInit;
  }
  

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
  lcd.print("C");
  dataString += thermoc;
  dataString += ",";
  

  //Voltage display
 
  float calcVolt = findVoltage(vPow,r1,r2); 
  lcd.setCursor(0,4);
  lcd.print(" VM:  ");
  lcd.print(calcVolt);
  lcd.print("V");
  dataString += calcVolt;
  dataString += ",";

  //Analog temperature sensor

  float anlTemp = analogueTemp(); 
  lcd.setCursor(0,5);
  lcd.print(" AT:  ");
  lcd.print(anlTemp);
  lcd.print("C");
  dataString += anlTemp;
  //dataString += ",";
  
  csvFile = SD.open(folderName, FILE_WRITE);
  csvFile.println(dataString);
  csvFile.close();
  
  Serial.println(dataString);


}

// -----------------------END OF LOOP----------------------- //



// ------------------------FUNCTIONS------------------------ //

// ----------Sensor Setup---------- //

void sensorSetup(void) {
  
  String dataString = "";
  
  totalDevices = discoverOneWireDevices();     // get addresses of our one wire devices into allAddress array 

  for (byte i=0; i < totalDevices; i++) {            // find out the number of total temp sensors
  sensors.setResolution(allAddress[i], 12);       // and set the a to d conversion resolution of each
  srlcd.setCursor(0,1);
  srlcd.print("Assign sensor: ")
  srlcd.print(sensors.getTempC(allAddress[i]));
  
  dataString += "DT";
  dataString += "(C)";
  dataString += ",";
  }

  if(thermocouple.readCelsius() != NAN) {
    dataString += "TC (C)";
    dataString += ",";
  }

  //if(analogRead(2) > 0) {
    dataString += "VM (V)";
    dataString += ",";
  //}

  if(analogRead(4) > 0) {
    dataString += "AT (C)";
    dataString += ",";
  }
  
  Serial.write("setup1");
  Serial

}

// -------End of Sensor Setup------ //

// --------Prev Sensor Setup------- //

void prevSensorInit(void) {
  
}

// ----End of Prev Sensor Setup---- //

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
