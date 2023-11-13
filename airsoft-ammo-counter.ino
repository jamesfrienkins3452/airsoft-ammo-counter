// Arduino board - Arduino Nano AT328

// Display module - 4-Digit LED Dispaly - Arduino Library "TM1637.h"
// Library link - https://github.com/avishorp/TM1637
// CLK pin - D2 pin Arduino
// DIO pin - D3 pin Arduino
// GND pin - GND pin Arduino
// 5V pin - 5V pin Arduino

// Button
// First pin - 5V Arduino
// Second pin - D4 Arduino

// Flying Fish IR Sensor
// GND pin - GND pin Arduino
// VCC pin - 5V pin Arduino
// Out pin - D5 pin Arduino


#include <Arduino.h>
#include <EEPROM.h>
#include <TM1637Display.h>
#include <ezButton.h>

//#include "libraries\TM1637-master\TM1637Display.h"
//#include "libraries\button-master\src\ezButton.h"


#define CLK 2
#define DIO 3
#define Button 4
#define Sensor 5

// display code
TM1637Display display(CLK, DIO);

const uint8_t data[] = { 0xff, 0xff, 0xff, 0xff }; // value used to put all segments on for testing purpose
const uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 }; // value used to put all segments off for testing purpose

// ammo count code
bool previsiousBulledStatus = true;
bool bulletDetected = false;
int default_ammo_count = 300; // default number of ammo in magazine
int ammo_count = 300;
const int EEPROM_address = 0;  // EEPROM address to store the default number of ammo count

byte change_default_ammo_count_state = 0; // 0 - count isn't changing; 1 - count is increasing; 2 - count is decreasing; 3 - count should be saved

// button code
const int SHORT_PRESS_TIME = 2000; // 1000 milliseconds
const int LONG_PRESS_TIME  = 2000; // 1000 milliseconds

ezButton button(Button);  // create ezButton object that attach to pin 7;

unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;


void setup() {
  Serial.begin(9600);
  pinMode(Sensor, INPUT);
  button.setDebounceTime(50); // set debounce time to 50 milliseconds

  // ammo count code
  const byte storedValue = EEPROM.read(EEPROM_address);
  if (storedValue == 255) {   // 255 is the default value for uninitialized EEPROM
    EEPROM.write(EEPROM_address, default_ammo_count);
  } else {
    default_ammo_count = storedValue;
  }
  ammo_count = default_ammo_count; // current ammo count in magazine
  Serial.println("setup() function finished");
}


void loop() {
  button.loop(); // MUST call the loop() function first

  // button code
  if (button.isPressed())
    pressedTime = millis();

  if (button.isReleased()) {
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;

    if (pressDuration <= SHORT_PRESS_TIME) {    // short press - reset ammo count
      Serial.println("A short press is detected");
      quickPress();
    }
    if (pressDuration > LONG_PRESS_TIME ){   // long press - change default ammo count
      Serial.println("A long press is detected");
      longPress();
    }
  }

  // ammo code
  if (change_default_ammo_count_state == 0) {
    bulletDetected = bulletDetection();
    if (bulletDetected == true and previsiousBulledStatus == false) {
      ammo_count -= 1;
      display_number(ammo_count);
      previsiousBulledStatus = true;
    } else {
      previsiousBulledStatus = false;
    }
  }

}


void display_number(int disp_number) {    // function to display value
  display.showNumberDec(disp_number, true);  // Expect: 0300
}


void quickPress() {   // button pressed quickly - number of ammo should be reset / defalut value should be changed
  if (change_default_ammo_count_state == 0) {   // just normal shooting mode
    ammo_count -= 1;
    display_number(ammo_count);
    Serial.println("Reseting ammo count to default");
  } else if (change_default_ammo_count_state == 1) {    // increasing default ammo count
    default_ammo_count += 1;
    display_number(default_ammo_count);
    Serial.println("Increasing default ammo count by 1");
  } else if (change_default_ammo_count_state == 2) {    // decreasing default ammo count
    default_ammo_count -= 1;
    display_number(default_ammo_count);
    Serial.println("Decreasing default ammo count by 1");
  }
}


void longPress() {    // button pressed longly - number of default ammo should be changed
  if (change_default_ammo_count_state == 0) {   // going from normal shooting mode to increasing ammo count
    change_default_ammo_count_state = 1;
    display_number(default_ammo_count);
    Serial.println("Switched to mode of increasing default ammo count");
  } else if (change_default_ammo_count_state == 1) {    // going from increasing ammo count to decreasing
    change_default_ammo_count_state = 2;
    Serial.println("Switched to mode of decreasing default ammo count");
  } else if (change_default_ammo_count_state == 2){    // saving new default ammo value
    change_default_ammo_count_state = 0;
    EEPROM.write(EEPROM_address, default_ammo_count);
    display_number(ammo_count);
    Serial.println("Saving new default ammo count and returning to shooting mode");
  }
}


bool bulletDetection() {    // code for detecting bullet with 'Flying Fish Sensor'
  if (digitalRead(Sensor) == LOW) {
    return true;
  }
  return false;
}
