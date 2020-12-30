# BikeComputerArduino

# What is this?
This project is for a fully functional cycling computer built from an Arduino and various sensors. Designed to connect to an Android device over bluetooth via this app:

## Features
* Wheel RPM
* Odometer
* Front/Rear Gear Indicator
* Low power consumption (about 30 hrs of use from 4x AA batteries)

## Planned Features
* Brake light control (MOSFET switching external power supply driving LED board)
* Brake sensors (Bump switches on brake levers?)
* Dynamic configuration via Android app (store settings in EEPROM)
* Automatic shifting
* Switch to Arduino Pro Micro (to reduce size and power consumption)

## Requirements (add links)
* Arduino Uno
* HC-05 Bluetooth module
* A3144 Hall effect sensor
* 1/4in Neodymium magnet (additional magnets optional) 
* 10k Slide potentiometers (45mm travel) (x2)
* 4xAA Battery holder w/ DC barrel jack
* 20+ ft 22AWG wire

## Wiring/Pinout Diagram
![Wiring Diagram](https://github.com/masranber/BikeComputerArduino/blob/main/images/arduino%20bike%20computer%20diagram.png)
