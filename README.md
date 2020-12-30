# BikeComputerArduino

# What is this?
This project is for a fully functional cycling computer built from an Arduino and various sensors. Designed to connect to an Android device over bluetooth via this app:

## Features
* Wheel speed
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

## Using (low effort, working on better guide w/ pictures)
### Attaching Sensors
1. Mount hall effect sensor to front fork
2. Attach magnet to spoke , securing with tape or epoxy, so it passes directly in front of the hall sensor when the wheel spins
3. Attach slide potentiometers to bike frame and shift cables (detailed below in Slide Potentiometer section)
4. Connect sensors to Arduino according to wiring diagram below
### Flashing Arduino
5. Connect Arduino to PC and flash the BikeComputerArduino.ino sketch using the Arduino IDE
6. Leave Arduino connected to USB so it's powered on
### Configuring Android Device
7. Install Bike Computer app on your device
8. In your device's settings, turn on bluetooth and pair with the HC-05
### Testing
7. Connect Arduino to power source (battery or USB)
8. Open Android app and press "Connect"

You should see the speed appear. Pressing "Start Trip" will begin a trip and you should see the odometer, average speed, and timer display.

## Arduino
An Arduino Uno R3 was used for this project. The pinouts can be configured for any Arduino provided it supports hardware interrupts.
Should work on 3.3V Arduino boards, however I did not test this. The A3144 Hall Effect Sensor datasheet specifies a minimum VCC of 4.5V however it should operate at 3.3V although the rise/fall times on the output might be lazy.

## Power
I used a AA battery holder with a DC barrel jack to power the Arduino. The batteries are wired in series to provide 6V to the Arduino. I think it goes without saying... DON'T wire the Arduino directly (via the 5V pin) to a battery pack with no voltage regulator. Alternatively, a USB power bank can be used. However beware many USB power banks will power off if current draw is too low (which is generally the case with Arduino projects).

## Sensors
### HC-05 Bluetooth Module
Required for communicating with an Android/iOS device. For configuring the HC-05 (device name, password, baudrate, etc...):
https://exploreembedded.com/wiki/Setting_up_Bluetooth_HC-05_with_Arduino#Configuring_with_AT_Commands

HC-05 is connected to the Arduino according to the pinout diagram below. The RXD/TXD pins of the HC-05 are misleading. Those correspond to the function of the carrier board. So RXD is data coming into the carrier board (aka data you're transmitting over BT from the Arduino) and vice versa for the TXD pin. Therefore, when the device is configured in code, the actual function of the Arduino serial pins is opposite what is stated on the HC-05's carrier board. Technically the maximum VCC for the HC-05 is +3.3V DC however it is safe to power it on +5V from the Arduino Uno. After 5 months it is still working fine.

### A3144 Hall Effect Sensor
Required for calculating wheel speed and odometer. The output is active low (output is pulled to GND when a magnetic field is present near the sensor) so the output must have either a 10k pullup resistor to +5V, or alternatively (the method used by this project) using the internal pullup resistor in the Arduino Uno (INPUT_PULLUP).

### 10kΩ Slide Potentiometer
Required for gear indicators. 10k is optimal, although anything from 1k-50k works. 45mm of travel is enough for most bikes. The potentiometer body can be ziptied to the frame beneath the shift cables. The easiest location to put these is along the top tube near the headset where both shift cables run next to each other. This minimizes the amount of wire needed. The slides were epoxied to the shift cables.

## Wiring/Pinout Diagram
![Wiring Diagram](https://github.com/masranber/BikeComputerArduino/blob/main/images/arduino%20bike%20computer%20diagram.png)
