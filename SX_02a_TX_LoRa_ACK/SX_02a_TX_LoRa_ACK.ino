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

/* Create an rtc object */
RTCZero rtc;

// Set up timer 
const byte alarmSeconds = 10;
const byte alarmMinutes = 0;
const byte alarmHours = 0;


volatile bool alarmFlag = true; // Start awake

int e;

char message1 [] = "Packet 1, wanting to see if received packet is the same as sent packet";
char message2 [] = "Packet 2, broadcast test";

void setup()
{
  delay(5000);
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  pinMode(CS,OUTPUT);
  digitalWrite(CS,HIGH);

  // Set up the real time clock 
  rtc.begin(); // Set up clocks and such
  rtc.attachInterrupt(alarmMatch); // Set up a handler for the alarm

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
  if (alarmFlag == true) {

      alarmFlag = false;  // Clear flag
      
      // Send message1 and print the result
      e = sx1272.sendPacketTimeoutACK(8, message1);
      Serial.print(F("Packet sent, state "));
      Serial.println(e, DEC);
    
      delay(500);  
    
      // Send message2 broadcast and print the result
      e = sx1272.sendPacketTimeoutACK(0, message2);
      Serial.print(F("Packet sent, state "));
      Serial.println(e, DEC);
    
      delay(500);  
  }


  resetAlarm();  // Reset alarm before returning to sleep
  Serial.println("Alarm set, going to sleep now.");

  //puts SAMD21 to sleep
  digitalWrite(LED_BUILTIN, LOW);
  Serial.end();
  rtc.standbyMode(); //library call

}


//interrupt service routine (ISR), called when interrupt is triggered 
//executes after MCU wakes up
void alarmMatch(void)
{
  Serial.begin(9600);
  //delay(3000); // Wait for console
  Serial.println("Woke up!");
  digitalWrite(LED_BUILTIN, HIGH);
  alarmFlag = true; 
}

// Resets the alarm
void resetAlarm(void) {
  byte seconds = 0;
  byte minutes = 0;
  byte hours = 0;
  byte day = 1;
  byte month = 1;
  byte year = 1;
  
  rtc.setTime(hours, minutes, seconds);
  rtc.setDate(day, month, year);

  rtc.setAlarmTime(alarmHours, alarmMinutes, alarmSeconds);
  rtc.enableAlarm(rtc.MATCH_HHMMSS);
}


