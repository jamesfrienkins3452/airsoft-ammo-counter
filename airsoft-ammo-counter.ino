#include <Arduino.h>
#include <EEPROM.h>
#include <TM1637Display.h>
#include <ezButton.h>

#define CLK 2
#define DIO 3
#define Button 4
#define Sensor 5

// display code
TM1637Display display(CLK, DIO);

const uint8_t data[] = { 0xff, 0xff, 0xff, 0xff }; // value used to put all segments on for testing purpose
const uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 }; // value used to put all segments off for testing purpose

// ammo count code
int defaultAmmoCount = 300;
int ammoCount = 300;
bool previousBulletStatus = true;
bool bulletDetected = false;
const int EEPROM_Address = 0;

byte changeDefaultAmmoCountState = 0; // 0 - count isn't changing; 1 - count is increasing; 2 - count is decreasing; 3 - count should be saved

// button code
ezButton button(Button);

unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
const int LONG_PRESS_TIME  = 2000;

void display_number(int displayNumber) {
  display.showNumberDec(displayNumber, true);
}


void quickPress() {   // button pressed quickly - number of ammo should be reset / defalut value should be changed
  if (changeDefaultAmmoCountState == 0) {   // just normal shooting mode
    ammoCount --;
    display_number(ammoCount);
    Serial.println("Reseting ammo count to default");
  } else if (changeDefaultAmmoCountState == 1) {    // increasing default ammo count
    defaultAmmoCount ++;
    display_number(defaultAmmoCount);
    Serial.println("Increasing default ammo count by 1");
  } else if (changeDefaultAmmoCountState == 2) {    // decreasing default ammo count
    defaultAmmoCount --;
    display_number(defaultAmmoCount);
    Serial.println("Decreasing default ammo count by 1");
  }
}


void longPress() {    // button pressed longly - number of default ammo should be changed
  if (changeDefaultAmmoCountState == 0) {   // going from normal shooting mode to increasing ammo count
    display_number(defaultAmmoCount);
    Serial.println("Switched to mode of increasing default ammo count");
  } else if (changeDefaultAmmoCountState == 1) {    // going from increasing ammo count to decreasing
    Serial.println("Switched to mode of decreasing default ammo count");
  } else if (changeDefaultAmmoCountState == 2){    // saving new default ammo value
    EEPROM.write(EEPROM_Address, defaultAmmoCount);
    display_number(ammoCount);
    Serial.println("Saving new default ammo count and returning to shooting mode");
  }
  changeDefaultAmmoCountState = (changeDefaultAmmoCountState + 1) % 3;
}


bool bulletDetection() {    // code for detecting bullet with 'Flying Fish Sensor'
  return (digitalRead(Sensor) == LOW) ? true : false;
}


void setup() {
  Serial.begin(9600);
  pinMode(Sensor, INPUT);
  button.setDebounceTime(50); // set debounce time to 50 milliseconds

  // ammo count code
  const byte storedValue = EEPROM.read(EEPROM_Address);
  if (storedValue == 255) {   // 255 is the default value for uninitialized EEPROM
    EEPROM.write(EEPROM_Address, defaultAmmoCount);
  } else {
    defaultAmmoCount = storedValue;
  }
  ammoCount = defaultAmmoCount; // current ammo count in magazine
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

    if (pressDuration <= LONG_PRESS_TIME) {    // short press - reset ammo count
      Serial.println("A short press is detected");
      quickPress();
    }
    if (pressDuration > LONG_PRESS_TIME ){   // long press - change default ammo count
      Serial.println("A long press is detected");
      longPress();
    }
  }

  // ammo code
  if (changeDefaultAmmoCountState == 0) {
    bulletDetected = bulletDetection();
    if (bulletDetected && !previousBulletStatus) {
      ammoCount --;
      display_number(ammoCount);
    previousBulletStatus = (bulletDetected && !previousBulletStatus);
    }
  }
}
