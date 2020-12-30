#include <SoftwareSerial.h>

// Configured for an Arduino Uno R3

// Digital pin connections
const int BT_RX_PIN_DIG = 10;    // DIGITAL PIN 10, Bluetooth RX
const int BT_TX_PIN_DIG = 11;    // DIGITAL PIN 11, Bluetooth TX
const int HALL_SPD_PIN_DIG = 2;  // DIGITAL PIN 2, Hall effect speed sensor

// Analog pin connections
const int FR_GEAR_PIN_AN = 1;    // ANALOG PIN 1, Front gear position sensor
const int R_GEAR_PIN_AN = 0;     // ANALOG PIN 0, Rear gear position sensor

SoftwareSerial BTSerial(10, 11); // RX | TX

// TODO Add dynamically configuring magnets from android app
const int MAG_PER_WHEEL = 1;              // Number of magnets per wheel (spaced evenly), more = higher RPM refresh rate (better low speed sensitivity), less = less noise in RPM calculation
const int MAX_REFRESH_INT = 200;          // Maximum refresh interval of the computer output (in milliseconds)

volatile long lifetimeRotations = 0;      // Total rotations counter since Arduino power up
volatile long currentRpm = 0;             // Stores current calculated RPM, volatile since this is assigned by an interrupt handler

bool inCalibrationMode = false;

// TODO Add dynamic calibration, this will require writing to EEPROM to store the calibration values
// Define the calibrations for the gear indicator. Analog values represent the slide potentiometer position in gear (index+1)
// Valid analog values from 0-1023
// EACH BIKE WILL REQUIRE ITS OWN CALIBRATION VALUES!!!
int rGearCalib[7] = {900, 790, 660, 530, 420, 300, 140};
int frGearCalib[3] = {0,0,0};   // TODO 
const int GEAR_TOLERANCE = 50;  // Tolerance allowed in gear analog value (unless derailer/shift cable is very worn/loose this should never need to be adjusted)

static volatile unsigned long last_interrupt_time = 0;

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  pinMode(2, INPUT_PULLUP);     // Hall effect sensor is active low so we should enabled Arduino's internal pull up resistor on the signal pin
  attachInterrupt(digitalPinToInterrupt(HALL_SPD_PIN_DIG), hall_detect, FALLING); // Hall effect sensor is active low so we want a HW interrupt that triggers on falling edge (aka a magnetic flux is present)
}

void loop() {
  writeData(currentRpm, lifetimeRotations, getFrontGear(), getRearGear()); // Front and rear gear always a positive integer 0-255 so we can use a byte to save memory and make serializing easier
  delay(MAX_REFRESH_INT);
}

/*
 * Calculates which gear the bike's rear cassette is currently in
 * Reads the value of a slide potentiometer attached to shift cable
 * and compares it to calibrated values for each gear
 * returns: byte - the current rear gear (indexed starting at 1 (lowest gear)) as an unsigned 8 bit integer
 */
byte getRearGear() {
  int analogPot = analogRead(R_GEAR_PIN_AN);
  for(byte i = 0; i < 7; i++) { // For each gear position, TODO replace loop bounds with len(gear calibration array)
    // Check if current sensor value is within range for specified gear
    if(inRange(analogPot, rGearCalib[i], GEAR_TOLERANCE)) {
      return i+1;
    }

  }
  return 0;
}

/*
 * Calculates which gear the bike's crankset is currently in
 * Reads the value of a slide potentiometer attached to shift cable
 * and compares it to calibrated values for each gear
 * returns: int - the current front gear (indexed starting at 1 (lowest gear)) as an unsigned 8 bit integer
 */
byte getFrontGear() {
  return 0;
}

/*
 * Writes the given data to the bluetooth module in the form of a serial data packet (big endian)
 * Writes the data to the BT module regardless of it's connection status
 * rpm and rotations should technically be unsigned ints but since the target platform is Android the conversion to Java datatypes is easier if they match nicely
 *
 *                                      DEVICE DATA PACKET (9 bytes)
 *       |   START   |                             DATA (8 bytes)                                 |
 *       |  char '$' |  int16 rpm  |  int32 lifetime rotations  |  uint8 gear F  |  uint8 gear R  |
 *          1 byte       2 bytes               4 bytes                1 byte          1 byte
 */
void writeData(int rpm, long totalRot, byte frontGear, byte rearGear) {
  byte data[9];                              // Each data packet is 9 bytes
  data[0] = '$';                             // A $ marks the beginning of a data packet
  data[1] = (byte) rpm >> 8;                 // Serialize int (2 bytes/16 bits) in big endian order using bit shifting
  data[2] = (byte) rpm;                      
  data[3] = (int)((totalRot >> 24) & 0xFF);  // Serialize long (4 bytes/32 bits) in big endian order using bit shifting/masking
  data[4] = (int)((totalRot >> 16) & 0xFF);
  data[5] = (int)((totalRot >> 8) & 0XFF);
  data[6] = (int)((totalRot & 0XFF));
  data[7] = (byte) rearGear;                 // Explicit cast to byte optional
  data[8] = (byte) frontGear;
  
  for(int i = 0; i < sizeof(data); i++){
    BTSerial.write(data[i]);                 // Write data packet byte by byte (big endian) to bluetooth module
  }
}

/*
 * Interrupt handler for the hall effect sensor
 * Updates the current RPM and total rotations
 */
void hall_detect() {
 
  unsigned long interrupt_time = millis();
  
  // Debounce of 10ms to remove noise
  if (interrupt_time - last_interrupt_time > 10) {
  
    long rotTime = (millis() - last_interrupt_time) * MAG_PER_WHEEL; // Time for a single rotation is time since last magnet detected times # magnets on the wheel
    int rpm = (1000.0 / rotTime) * 60;
    
    // Perform a sanity check on the calculated value, 600 rpm on a 28 inch wheel is 50 mph (bruh ain't nobody pedaling a bike this fast), helps remove noise
    if(rpm < 600 && rpm > 0) { 
      currentRpm = rpm;
      lifetimeRotations++;     // TODO Fix rotation counter with multiple magnets on wheel
    }
  }
  last_interrupt_time = interrupt_time;
}

/*
 * Determines if the actual value is within a specified tolerance from the ideal value
 * int actual: actual value to compare
 * int ideal: ideal value to compare against
 * int tolerance: maximum range +- actual value must be from ideal value
 * returns bool: true if actual value is within tolerance, false otherwise
 */
bool inRange(int actual, int ideal, int tolerance) {
  return ((ideal - tolerance) <= actual) && (actual <= (ideal + tolerance));
}

/*
 * TODO Fix
 * Computes the length of an array of integers
 * int[]: the array of integers to determine the length of
 * return int: length of the array (# of indices)
 */
int len(int intArr[]) {
  return sizeof(intArr) / sizeof(int);
}
