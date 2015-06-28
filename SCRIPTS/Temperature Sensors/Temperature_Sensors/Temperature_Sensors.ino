/*
  Temperature Sensors
 This code will read the temperature from two temperature sensors:
   Dallas 18820
   TMP36
 */

#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 3 on the Arduino
#define ONE_WIRE_BUS 3

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// Insert digital thermometer device IDs
DeviceAddress digiThermometer = { 0x28, 0xFF, 0xAC, 0x2C, 0x4E, 0x04, 0x00, 0xC6 };

// the setup routine runs once when you press reset:
void setup(void) {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  //start the sensors library
  sensors.begin();
  // set the resolution to 12 bit
  sensors.setResolution(digiThermometer, 12);
}

float analogueTemp()
{
  // read the input on analog pin 0 (TMP36 input):
  int anlsensorValue = analogRead(A0);
  
  // Convert the input signal into a voltage
  float voltage = anlsensorValue * 5.0;
  voltage /= 1024.0;
  
  // convert the sensor value to temp in C
  float anlTemp = (voltage - 0.5)*100;
  return anlTemp;
}

float digitalTemp(DeviceAddress deviceAddress)
{
  sensors.requestTemperatures();
  // read the digital temperature sensor value
  float dgtlTemp = sensors.getTempC(deviceAddress);
  
  // if there is -127, there is no value being read, hence:
  if (dgtlTemp == -127.00) {
    Serial.print("Error getting temperature");
  }
  // Delay to allow processing time
  return dgtlTemp;
}

void loop() {

  // -------- Analog temperature sensor -------- //

  // print out the temperature value you read:
  float anlTemp = analogueTemp();  

  // -------- Analog temperature sensor -------- //
  
  // -------- Digital temperature sensor -------- //
  
  float dgtlTemp = digitalTemp(digiThermometer);
  
  // -------- Digital temperature sensor -------- //
  
  Serial.print("Analogue Temperature:");
  Serial.print(anlTemp);
  Serial.println(" C");
  Serial.println("");
  
  Serial.print("Digital Temperature:");
  Serial.print(dgtlTemp);
  Serial.println(" C");
  Serial.println(""); 
  delay(2000);
}
