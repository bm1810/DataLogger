
/*
 * Script to identifty the connected onewire devices and assign them a type.
 * This data will be stored on an SD card and accessed by the datalogging program
 * The config file with device IDs can be shared between data loggers if the sensors
 * will be shared also.
 *
 * !! Only connect a single OneWire device at once !!
 *
 * NOTE; Devices types:
 * 1 DS18B20 Digital temperature sensor
 * 2 MAX31850 thermocouple
 * 3 DS2438 Humidity sensor
 * 4 DS2438 Light sensor
 * 5 DS2438 Pressure sensor
 * 6 DS2438 Proximity sensor
 */

// Onewire sensor libraries
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DS2438.h>
// SPI library
#include <SPI.h>
// LCD Libraries
#include <TFTv2.h>
// SD library
#include <Fat16.h>


// ---------------------------Tft SETUP------------------------ //

byte lineNo = 0;

// Sensor menu options - store in program memory
const char sens1[] PROGMEM = "1 DS18B20 Digital Temperature Sensor";
const char sens2[] PROGMEM = "2 MAX31850 Thermocouple";
const char sens3[] PROGMEM = "3 DS2438 Humidity Sensor";
const char sens4[] PROGMEM = "4 DS2438 Light Sensor";
const char sens5[] PROGMEM = "5 DS2438 Pressure Sensor";
const char sens6[] PROGMEM = "6 DS2438 Proximity Sensor";

// Setup table to refer to strings
const char* const string_table1[] PROGMEM = {sens1, sens2, sens3, sens4, sens5, sens6};

// Displayed messages

const char mess1[] PROGMEM = "Insert SD card";
const char mess2[] PROGMEM = "Error: Fat16 init";
const char mess3[] PROGMEM = "Reboot";
const char mess4[] PROGMEM = "then";
const char mess5[] PROGMEM = "Attach single sensor";
const char mess6[] PROGMEM = "only";
const char mess7[] PROGMEM = "Onewire sensor detected";
const char mess8[] PROGMEM = "Press OK to confirm";

// Setup table to refer to strings
const char* const string_table2[] PROGMEM = {mess1, mess2, mess3, mess4, mess5, mess6, mess7, mess8};

// Buffer to hold text temporarily
char buffer[35];
// Buffer to hold line of config text
char infobuf[50];

// ----------------------END OF Tft SETUP---------------------- //


// ------------------------Button SETUP------------------------ //

// Define the button pins
const int bPin1 = A0;
const int bPin2 = A1;
const int bPin3 = A2;

// ---------------------End of button SETUP-------------------- //

// ---------------------------SD SETUP------------------------- //

// Set select pin for SD card
#define CHIP_SELECT     4 // SD chip select pin
//Setup folder names
SdCard card;
Fat16 file;
char name[] = "config";

// ----------------------END OF SD SETUP----------------------- //

// -------------------------Onewire SETUP---------------------- //

// Data wire is plugged into data pin 4 on the Arduino
#define ONE_WIRE_BUS 3

//Define the max number of sensors attached
#define NumberOfDevices 4

// Setup a matrix in which to store digital sensor addresses
byte allAddress [NumberOfDevices][8];
// Declare variable to store number of One Wire devices
byte totalDevices;

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// ---------------------END OF Onewire SETUP------------------ //

void setup() {

  // Setup button pins
  pinMode(bPin1, INPUT);
  pinMode(bPin2, INPUT);
  pinMode(bPin3, INPUT);

  // Initiate Tft
  TFT_BL_ON;
  Tft.TFTinit();
  Tft.fillScreen(0, 100, 0, 100, WHITE);

  // see if the card is present and can be initialized:
  if (!card.begin(CHIP_SELECT)) {
    Tft.drawString("Insert SD card and reboot", 5, 10, 1, BLACK);
    delay(2000);
    // don't do anything more - infinite while loop
    cli();
    abort();
  }

  // initialize a FAT16 volume;
  if (!Fat16::init(&card)) {
    Tft.drawString("Error: Fat16 init", 5, 5, 1, BLACK);
    Tft.drawString("Reboot to try again", 5, 10, 1, BLACK);
    // don't do anything more - infinite while loop
    cli();
    abort();
  }

  // Create a config file on the SD card
  file.open(name, O_CREAT | O_APPEND | O_WRITE);
  file.close();

}

void loop() {

  // search for one wire devices and copy to device address array
  byte j = 0;
  while ((j < NumberOfDevices) && (oneWire.search(allAddress[j]))) {
    j++;
  }
  if (j = 0) {
    Tft.fillScreen(0, 100, 0, 100, WHITE);
    Tft.drawString("Attach sensor", 5, 5, 1, BLACK);
    delay (1000);
  } else if (j > 1) {
    Tft.fillScreen(0, 100, 0, 100, WHITE);
    Tft.drawString("Attach only 1 sensor", 5, 5, 1, BLACK);
    delay (1000);
  } else if (j = 1) {
    // 1st, check if the sensor has been added to the database before

    //Setup a line buffer
    char line[12];
    int n;

    // Check to see if the config file exists and if the ID of the discovered sensor is present
    if (file.open(name, O_READ)) {
      // read lines from the file
      while ((n = file.fgets(line, sizeof(line))) > 0) {
        if (line[n - 1] == '\n') {
          // remove LF
          line[n - 1] = 0;
        }

        byte i;
        byte present = 0;
        byte data[12];
        byte addr[8];

        Serial.print("Looking for 1-Wire devices...\n\r");
        while (ds.search(addr)) {
          Serial.print("\n\rFound \'1-Wire\' device with address:\n\r");
          for ( i = 0; i < 8; i++) {
            Serial.print("0x");
            if (addr[i] < 16) {
              Serial.print('0');
            }
            Serial.print(addr[i], HEX);
            if (i < 7) {
              Serial.print(", ");
            }
          }
          if ( OneWire::crc8( addr, 7) != addr[7]) {
            Serial.print("CRC is not valid!\n");
            return;
          }
        }
        Serial.print("\n\r\n\rThat's it.\r\n");
        ds.reset_search();

        sprintf(infobuf, "%02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X,%d,", allAddress[1][1], allAddress[2][1], allAddress[3][1], allAddress[4][1], allAddress[5][1], allAddress[6][1], allAddress[7][1], allAddress[8][1], i);
        byte IDbuf[8];
        int stype;
        sscanf(line, "%[^','],%[^',']", IDbuf, stype);
        if (ByteArrayCompare(IDbuf, allAddress[j], 8) == 0) {
          Tft.drawString("Sensor already exists", 5, 5, 1, BLACK);
          delay(5000);
          break;
        }

        Tft.fillScreen(0, 100, 0, 100, WHITE);
        Tft.drawString("Sensor detected", 5, 5, 1, BLACK);

        file.open(name, O_CREAT | O_APPEND | O_WRITE);
        file.println();
        int i = 1;
        strcpy_P(buffer, (char*)pgm_read_word(&(string_table[i])));
        Tft.drawString(buffer, 5, 10, 1, BLACK);
        while (1 == 1) {
          if (bPin1 == HIGH) {
            i--;
            if (i = 0) (i = 6);
            Tft.drawString(buffer, 5, 10, 1, WHITE);
            strcpy_P(buffer, (char*)pgm_read_word(&(string_table[i])));
            Tft.drawString(buffer, 5, 10, 1, BLACK);
          } else if (bPin2 == HIGH) {
            Tft.fillRectangle(0, 240, 7, 10, GRAY1);
            Tft.drawString(buffer, 5, 10, 1, BLACK);
            break;
          } else if (bPin3 == HIGH) {
            i++;
            if (i = 7) (i = 0);
            Tft.drawString(buffer, 5, 10, 1, WHITE);
            // Recall from program memory the type of sensor (text)
            strcpy_P(buffer, (char*)pgm_read_word(&(string_table[i])));
            // display on the screen
            Tft.drawString(buffer, 5, 10, 1, BLACK);
          }
        }
        Tft.drawString("Press OK to confirm selection", 5, 5, 1, BLACK);
        //Wait for user to press OK again
        while (1 == 1) {
          if (bPin2 == HIGH) {
            break;
          }
        }
        Tft.drawString("Saving...", 5, 20, 1, BLACK);
        //store the Onewire 64 bit address, the type number on each line in config file
        sprintf(infobuf, "%02X, %02X, %02X, %02X, %02X, %02X, %02X, %02X,%d,", allAddress[1][1], allAddress[2][1], allAddress[3][1], allAddress[4][1], allAddress[5][1], allAddress[6][1], allAddress[7][1], allAddress[8][1], i);
        // Print this information to a new line on the SD card
        file.println(infobuf);
      }
    }
  }
}

// ---------- Functions ---------- //
boolean ByteArrayCompare(byte a[], byte b[], int array_size)
{
  for (int i = 0; i < array_size; ++i) {
    if (a[i] != b[i]) {
      return (false);
    }
  }
  return (true);
}

/*
void discoverOneWireDevices(void) {
  byte i;
  byte present = 0;
  byte data[12];
  byte addr[8];

  // Look for onewire devices
  while(oneWire.search(addr)) {

    for( i = 0; i < 8; i++) {
      Serial.print("0x");
      if (addr[i] < 16) {
        Serial.print('0');
      }
      Serial.print(addr[i], HEX);

      if (i < 7) {
        Serial.print(", ");

      }
    }
    if ( OneWire::crc8( addr, 7) != addr[7]) {
        Serial.print("CRC is not valid!\n");
        return;
    }
  }
  Serial.print("\n\r\n\rThat's it.\r\n");
  ds.reset_search();
  return;
}
*/

// I have copied in the onewire search. I think the address should be store in the 0x format on the SD card which will allow for easy importing into the Dallas temp code
