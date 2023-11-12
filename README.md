# ammo-count-detector
Arduino based project that can be used to count ammo in airsoft guns

Modules required:
- Arduino
- 4-Digit LED Dispaly
- Flying Fish IR Sensor
- Button
- Power supply

Libraries used:
- https://github.com/avishorp/TM1637
- https://github.com/ArduinoGetStarted/button

How to correctly connect pins:

| 4-Digit LED Dispaly | Arduino       |
|--------------------:|---------------|
| CLK                 | D2            |
| DIO                 | D3            |
| GND                 | DNG           |
| 5V                  | 5V            |

| Button | Arduino                      |
|--------------------:|-----------------|
| Pin 1                 | 5V            |
| Pin 2                 | D4            |

| Flying Fish IR Sensor | Arduino       |
|----------------------:|---------------|
| Out                   | D5            |
| GND                   | DNG           |
| VCC                   | 5V            |

Quick illustration of project
![illustration](https://github.com/jamesfrienkins3452/airsoft-ammo-counter/assets/80784310/6cd799dc-b9af-4b38-988a-9ad7f650665a)
