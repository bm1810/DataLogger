/*
  Temperature Sensors
 This code will read the temperature from a Dallas 18820 temperature sensor and type K thermocouple connected to a MAX6675
 record the value with a real time.
   
 */

// Temperature sensor libraries
#include <OneWire.h>
#include <DallasTemperature.h>
// Timer libraries
#include <Wire.h>
#include <DS3231.h>
//MAX6675 Library (thermocouple)
#include <max6675.h>

//Set up the pins that the MAX6675 connects to
int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

//Define what the thermocouple temperature will be
int tctemp = 0;

//Bring the in the real time clock functions
DS3231 Clock;
byte year, month, date, DoW, hour, minute, second;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;
bool Century=false;

//Setup millisec value
volatile unsigned long millistart;

// Data wire is plugged into data pin 3 on the Arduino
#define ONE_WIRE_BUS 3

//Define an alarm every second and every minute
#define ALRM1_ONCE_PER_SEC  0b1111  // once a second
#define ALRM2_ONCE_PER_MIN  0b111   // once per minute (00 seconds of every minute)
byte ALRM1_SET = ALRM1_ONCE_PER_SEC;
byte ALRM2_SET = ALRM2_ONCE_PER_MIN;

//Define the counting value
volatile unsigned long _millistart;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
// Insert digital thermometer device ID
DeviceAddress digiThermometer = { 0x28, 0xFF, 0xAC, 0x2C, 0x4E, 0x04, 0x00, 0xC6 };

//wSetup the RTC alarm
void setAlarms() {
   
 // Set AlarmBits, ALRM2 first, followed by ALRM1
     int AlarmBits = ALRM2_SET;
     AlarmBits <<= 4;
     AlarmBits |= ALRM1_SET;
     
  // set both alarms to every second
     Clock.setA1Time(Clock.getDoW(), Clock.getHour(h12, PM), Clock.getMinute(), AlarmBits, true, false, false);
     // Turn alarms on
     Clock.turnOnAlarm(1);
}

//Define the second interrupt from the RTC
void SecIntrpt() {
  //Define the chip value of milliseconds at the start of the count
  millistart = millis();
}


// the setup routine runs once when you press reset:
void setup(void) {
  // Start the I2C interface
  Wire.begin();
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  //start the sensors library
  sensors.begin();
  // set the resolution to 12 bit
  sensors.setResolution(digiThermometer, 12);
  //Set the status of the interrupt pin
  pinMode (2, INPUT);
  //Include an interrupt from the clock
  attachInterrupt(2, SecIntrpt, RISING);
  //Print some intro text
  Serial.println("DS18b20 Temperature Sensor");
  // Get and print the date from the RTC
  Clock.getTime(year, month, date, DoW, hour, minute, second);
  Serial.print(date, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.println(year, DEC);
  setAlarms();
  
  // wait for everything to stabilize
  delay(500);
  
}

void loop() {
  
  //Get the Temperature data from all temperature sensors on the bus1
  sensors.requestTemperatures();
  
  // Get the time
  Clock.getTime(year, month, date, DoW, hour, minute, second);
  int RTCmillisecs = millis() - millistart;
  
  // Print the time and the temperature values
  Serial.print(hour, DEC);
  Serial.print(minute, DEC);
  Serial.print(second, DEC);
  Serial.print(":");
  Serial.println(RTCmillisecs);
  Serial.println(sensors.getTempC(digiThermometer));
  
  //Get the Temperature data from all temperature sensors on the bus1
  sensors.requestTemperatures();
  tctemp = thermocouple.readCelsius()
  
  // Get the time
  Clock.getTime(year, month, date, DoW, hour, minute, second);
  int RTCmillisecs = millis() - millistart;
  
  // Print the time and the temperature values
  Serial.print(hour, DEC);
  Serial.print(minute, DEC);
  Serial.print(second, DEC);
  Serial.print(":");
  Serial.println(RTCmillisecs);
  Serial.print("Digital:");
  Serial.println(sensors.getTempC(digiThermometer));
  Serial.print("Thermocouple:");
  Serial.println(tctemp);
  
  delay(1000);
}
