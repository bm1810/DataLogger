/*
  Temperature Sensors
   This code will read the temperature from one temperature sensor:
   TMP36
 */
 
// the setup routine runs once when you press reset:
void setup(void) {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(57600);
}

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

void loop() {

  // -------- Analog temperature sensor -------- //

  // print out the temperature value you read:
  float anlTemp = analogueTemp();  

  // -------- Analog temperature sensor -------- //
  
  Serial.println(millis());
  Serial.println(anlTemp);
  
}
