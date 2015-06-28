/*

  Title:    LED PWM
  Author:   Ben Moffatt
  Date:     24/02/2015
  Version:  0.1

  Description:
    Use puttons to control the brightness of an LED using PWM output

  Acknowledgements:
    
  
*/

// Set the pin numbers
int ledPin = 9;
int buttonPin1 = A0;
int buttonPin2 = A1;

// Set the button states
int buttonState1 = 0;
int buttonState2 = 0;

// Set up the brightnesses
int brightness = 127;
int i=0;

// -----------------------OVERALL SETUP----------------------- //

void setup(void) {
  Serial.begin(9600);		                                   // set up for error checking	
  
  // Set up the pin modes
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  analogWrite(ledPin, brightness);
}


// --------------------END OF OVERALL SETUP------------------- //

// ---------------------------LOOP--------------------------- //

void loop() {

  //Read the state of buttonPin1
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);

  
  if(buttonState1 != 0) {
    brightness = brightness + 1;
    if(brightness>=255) {
      for(int i=0; i<=3; i++) {
        delay(100);
        analogWrite(ledPin, 0);
        delay(100);
        analogWrite(ledPin, 255);
      }
      brightness = 254;
    }
    analogWrite(ledPin, brightness);
   }
  
  if(buttonState2 != 0) {
    brightness = brightness - 1;
    analogWrite(ledPin, brightness);
  }
  
  
  delay(10);
}

// -----------------------END OF LOOP----------------------- //

// ------------------------FUNCTIONS------------------------ //

// ---------------------END OF FUNCTIONS--------------------- //
