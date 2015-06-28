/*
DS3231_test.pde
Eric Ayars
4/11

Test/demo of read routines for a DS3231 RTC.

Turn on the serial monitor after loading this to check if things are
working as they should.

*/

#include <DS3231.h>
#include <Wire.h>

#define CLOCK_ADDRESS 0x68

DS3231 Clock;
bool Century=false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;

byte year, month, date, DoW, hour, minute, second;

void setup() {
	// Start the I2C interface
	Wire.begin();
	// Start the serial interface
	Serial.begin(57600);
}

void loop() {
	// send what's going on to the serial monitor.
	// raw output
        Wire.beginTransmission(CLOCK_ADDRESS);
	Wire.write(uint8_t(0x00));
	Wire.endTransmission();
	
	Wire.requestFrom(CLOCK_ADDRESS, 7);
        float second = bcdToDec(Wire.read());
        Serial.println(raw);
        
        delay(100);

}

