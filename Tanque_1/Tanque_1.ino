/*  
 *  LoRa 868 / 915MHz SX1272 LoRa module
 *  
 *  Copyright (C) Libelium Comunicaciones Distribuidas S.L. 
 *  http://www.libelium.com 
 *  
 *  This program is free software: you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation, either version 3 of the License, or 
 *  (at your option) any later version. 
 *  
 *  This program is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License 
 *  along with this program.  If not, see http://www.gnu.org/licenses/. 
 *  
 *  Version:           1.2
 *  Design:            David Gascón
 *  Implementation:    Covadonga Albiñana, Victor Boria, Ruben Martin
 */

#include <Wire.h>

// Cooking API libraries
//include "arduinoUtils.h"

// Include the SX1272 and SPI library:
#include "arduinoLoRa.h"
#include <SPI.h>
#include <RTCZero.h>

// Create an rtc object 
RTCZero rtc;

// Set up timer 
const byte timerSeconds = 10;
const byte timerMinutes = 0;
const byte timerHours = 0;

// This helps us figure out if the timer has gone off
volatile bool timerFlag = true; // Start awake

int e;

// The messages we are sending 
char message1 [] = "Packet 1";
char message2 [] = "Packet 2";

void setup() 
{
  
  delay(5000);
  
  Serial.begin(9600);  // Open serial communications and wait for port to open:

  // Setting pin to disable transmitter SPI connection until ready
  pinMode(CS,OUTPUT);
  digitalWrite(CS,HIGH);

  // Set up the real time clock 
  rtc.begin(); // Set up clocks and such
  rtc.attachInterrupt(timerMatch); // Set up a handler for the alarm

  // Set up LEDs to track sleep mode
  pinMode(LED_BUILTIN, OUTPUT); //set LED pin to output
  digitalWrite(LED_BUILTIN, LOW); //turn LED off
  
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

void loop(void)
{

  // Check if we are out of sleep 
  if (timerFlag == true) {

      timerFlag = false;  // Clear flag
      
      // Send message1 and print the result
      e = sx1272.sendPacketTimeoutACKRetries(8, message1);
      Serial.print(F("Packet sent, state "));
      Serial.println(e, DEC);
    
      delay(500);  
    
      // Send message2 broadcast and print the result
      e = sx1272.sendPacketTimeoutACKRetries(8, message2);
      Serial.print(F("Packet sent, state "));
      Serial.println(e, DEC);
    
      delay(500);  
  }


  resetTimer();  // Reset alarm before returning to sleep
  
  Serial.println("Going to sleep now.");
  digitalWrite(LED_BUILTIN, LOW);
    
  rtc.standbyMode(); //puts SAMD21 to sleep

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


