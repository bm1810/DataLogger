/*--------------------------------------------------------------
	Project:				Individual project datalogger  

	Code Modified from startingelectronics and udemy
	https://www.udemy.com/blog/arduino-voltmeter/
	http://startingelectronics.com/articles/arduino/measuring-voltage-with-arduino/

  Program:      voltmeter

  Description:  Reads value on analog input A0 and calculates
                the voltage assuming that a voltage divider
                network on the pin divides by 11.
  
--------------------------------------------------------------*/

// SETUP
// number of analog samples to take per reading
#define NUM_SAMPLES 10
// reference voltage for the Arduino - MEASURE FOR ACCURACY
#define vPow 5.0
// resistance values - MEASURE FOR ACCURACY
#define r1 100000
#define r2 10000


int sum = 0;                    	// sum of samples taken
unsigned char sample_count = 0; 	// current sample number
float anavoltage = 0.0;         	// calculated voltage analogue pin sees
float voltage = 0.0;							// coltage before potential divider

void setup()
{
	 // Voltage will be read out on the serial monitor. Set it up.
   Serial.begin(9600);
	 // Introduce the voltmeter to the user, specifying the maximum voltage
   Serial.println("--------------------");
   Serial.println("DC VOLTMETER");
   Serial.print("Maximum Voltage: ");
   Serial.print((int)(vPow / (r2 / (r1 + r2))));
   Serial.println("V");
   Serial.println("--------------------");
   Serial.println("");

}

void loop()
{
    // take a number of analog samples and add them up
    while (sample_count < NUM_SAMPLES) {
        sum += analogRead(0);
        sample_count++;
        delay(10);
    }
    // calculate the real voltage
    anavoltage = ((float)sum / (float)NUM_SAMPLES * vPow) / 1024.0;
		voltage = anavoltage / ((float)r2 / ((float)r1 + (float)r2));
    Serial.print(voltage);
    Serial.println (" V");
    sample_count = 0;
    sum = 0;
}