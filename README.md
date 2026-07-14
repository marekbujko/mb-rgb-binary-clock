# RGB Binary Clock v1.2.9 for Arduino 🕓
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/license/mit/)
[![GitHub stars](https://img.shields.io/github/stars/marekbujko/mb-rgb-binary-clock?style=social)](https://github.com/marekbujko/mb-rgb-binary-clock/stargazers)

Try the RGB Binary Clock v1.2.9 Web UI with Arduino COM port support!
(On request)

<img src="Images/WebUI.gif"/>

This repository contains a modernized version of my original RGB Binary Clock project for the Arduino Nano, featuring numerous improvements and new functionality.


**Size**
* **Arduino Nano (ATmega328P):** 30 KB
* **Web UI:** 650 KB



**Hardware requirements**

* Arduino Nano, 6x RGB LEDs with resistors

* I2C: RTC DS3231/DS3232, LCD 16x2, TEA5767 (FM Radio), PCA8574 I2C IO expansion board

* Active buzzer, PS2 joystick


**Optional**

* I2C: SHT30 (temp & humidity), BMP280 (temp & pressure)

* RF: LoRa MeshCore Companion Radio Clients

The project outputs various data through the Serial Monitor at a baud rate of 115200 and includes a Web UI for complete control, with support for the Novation Launchpad X!

The Arduino firmware also features a built-in mini menu with FM radio, sensor monitoring, and Web UI control, operated using a PS2 joystick module.

## How does it actually work? 🔴🟢🔵🔴🟢🔵
The clock uses 6 RGB LEDs, where each LED represents a combination of hours, minutes, and seconds.

Each RGB LED consists of three color channels: red, green, and blue. By combining these colors, additional CMY and white color values can be created.

The 1st and 2nd LEDs are used only for representing minutes and seconds.

<img src="Images/rgb-binary-clock-logic.png"/>

For example, in the table above, the LED colors correspond to a specific time:
1. Green
2. Cyan
3. Magenta
4. Off
5. White
6. Blue

## How to read it 📖🧐

**Red** = hours

**Green** = minutes

**Blue** = seconds

Understanding the color combinations allows you to read the time visually.

The clock uses binary-weighted 6-bit representations for the RGB LEDs and operates in a 12-hour format.

Starting from the first (top) LED, the binary values are assigned as follows:

* **Minutes and seconds:** 1, 2, 4, 8, 16, 32
* **Hours:** starting from the 3rd LED: 1, 2, 4, 8


## License 📄
[MIT](LICENSE)
