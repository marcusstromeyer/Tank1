/*  
 *   
 *  Authors: Marcus and Louis 
 *  Description: Code for the tanque 1 
 *  Includes sensing, smoothing, standby and tramsmition
 *  
 */

// All the libraries we need 
#include <Wire.h>
#include <SPI.h>
#include <RTCZero.h>
#include "arduinoLoRa.h"

//include "arduinoUtils.h"

// Pins for the sensor 
#define trigPin 10
#define echoPin 11

// Create an rtc object 
RTCZero rtc;

// Set up timer 
const byte timerSeconds = 10;
const byte timerMinutes = 0;
const byte timerHours = 0;

// This helps us figure out if the timer has gone off
volatile bool timerFlag = true; 

// If we don't want a timer, set to true
const bool noTimer = true; 

// Transmition object
int e;

// Message header
char messageHeader [] = "Data: ";

void setup() 
{
  
  delay(5000);
  
  Serial.begin(9600);  // Open serial communications and wait for port to open:

  // Setting pin to disable transmitter SPI connection until ready
  pinMode(CS,OUTPUT);
  digitalWrite(CS,HIGH);

  // Define the inputs/outputs for distance sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Set up the real time clock 
  rtc.begin(); // Set up clocks and such
  rtc.attachInterrupt(timerMatch); // Set up a handler for the alarm

  // Set up LEDs to track sleep mode
  pinMode(LED_BUILTIN, OUTPUT); //set LED pin to output
  digitalWrite(LED_BUILTIN, LOW); //turn LED off

  setUpLora(); // Call function to set up the Lora
  
}

void loop(void)
{

  // Check if we are out of sleep 
  if (timerFlag == true || noTimer == true) {

      timerFlag = false;  // Clear flag

      int distanceFromSensor = getDistanceFromSensor(); // We get distance from sensor
      Serial.print("Distance from sensor: ");
      Serial.println(distanceFromSensor);
      // Put together char array to send
      char distanceChar[16];
      itoa(distanceFromSensor, distanceChar, 10);
      Serial.print("Distance Char: ");
      Serial.println(distanceChar);
      char dataToSend[sizeof(messageHeader) + sizeof(distanceChar) + 1];
      sprintf(dataToSend, "%s %s", messageHeader, distanceChar); // with word space

      Serial.println(dataToSend);

      // Send data 
      e = sx1272.sendPacketTimeoutACKRetries(8, dataToSend);
      Serial.print(F("Packet sent, state "));
      Serial.println(e, DEC);

      delay(500);  

  }

  if (noTimer!=true)
  {

    resetTimer();  // Reset alarm before returning to sleep
  
    Serial.println("Going to sleep now.");
    digitalWrite(LED_BUILTIN, LOW);
    
    rtc.standbyMode(); //puts SAMD21 to sleep
    
  }

}


//interrupt service routine (ISR), called when interrupt is triggered 
//executes after MCU wakes up
void timerMatch(void)
{
  
  Serial.begin(9600);
  Serial.println("Woke up!");
  digitalWrite(LED_BUILTIN, HIGH);
  timerFlag = true; 
  
}

void setUpLora(){

  // Print a start message
  Serial.println(F("SX1272 module and Arduino: send packets with ACK"));
  
  // Power ON the module
  e = sx1272.ON();
  Serial.print(F("Setting power ON: state "));
  Serial.println(e, DEC);
  
  // Set transmission mode and print the result
  e |= sx1272.setMode(1);
  Serial.print(F("Setting Mode: state "));
  Serial.println(e, DEC);
  
  // Set header
  e |= sx1272.setHeaderON();
  Serial.print(F("Setting Header ON: state "));
  Serial.println(e, DEC);
  
  // Select frequency channel
  e |= sx1272.setChannel(CH_04_900);
  Serial.print(F("Setting Channel: state "));
  Serial.println(e, DEC);
  
  // Set CRC
  e |= sx1272.setCRC_ON();
  Serial.print(F("Setting CRC ON: state "));
  Serial.println(e, DEC);
  
  // Select output power (Max, High or Low)
  e |= sx1272.setPower('H');
  Serial.print(F("Setting Power: state "));
  Serial.println(e, DEC);
  
  // Set the node address and print the result
  e |= sx1272.setNodeAddress(3);
  Serial.print(F("Setting node address: state "));
  Serial.println(e, DEC);
  
  // Print a success message
  if (e == 0)
    Serial.println(F("SX1272 successfully configured"));
  else
    Serial.println(F("SX1272 initialization failed"));
  
}

// Resets the alarm
void resetTimer(void) 
{
  byte seconds = 0;
  byte minutes = 0;
  byte hours = 0;
  byte day = 1;
  byte month = 1;
  byte year = 1;
  
  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);

  rtc.setAlarmTime(timerHours, timerMinutes, timerSeconds);
  rtc.enableAlarm(rtc.MATCH_HHMMSS);
}


// Uses ultrasonic sensor to get distance
int getDistanceFromSensor(){

 long duration; 
 int distance;
 
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);

  distance = (duration / 2) * 0.0344;
  
  if (distance >= 400 || distance <= 2){
    Serial.print("Distance = ");
    Serial.println(distance);
  }
  else {
    Serial.print("Distance = ");
    Serial.print(distance);
    Serial.println(" cm ");
  }

  return distance; 
  
}


