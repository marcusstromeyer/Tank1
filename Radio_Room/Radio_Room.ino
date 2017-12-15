
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Cooking API libraries
#include <arduinoUtils.h>

// Include the SX1272 and SPI library:
#include "arduinoLoRa.h"
#include <SPI.h>

int e;
char my_packet[100];

const int buttonPin = 5;     // the number of the pushbutton pin

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 20, 2);

volatile int buttonState = 0;         // variable for reading the pushbutton status

void setup()
{

  Serial.begin(9600);  // Open serial communications and wait for port to open:

  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  // Set up the lora receiver 
  setupLora(); 

  attachInterrupt(buttonPin, pin_ISR, CHANGE);

}

void loop()
{

  listenForData(); 

}

void setupLora()
{
  
  // Power ON the module
  e = sx1272.ON();
  Serial.print(F("Setting power ON: state "));
  Serial.println(e, DEC);
  
  // Set transmission mode and print the result
  e |= sx1272.setMode(4);
  Serial.print(F("Setting Mode: state "));
  Serial.println(e, DEC);
  
  // Set header
  e |= sx1272.setHeaderON();
  Serial.print(F("Setting Header ON: state "));
  Serial.println(e, DEC);
  
  // Select frequency channel
  e |= sx1272.setChannel(CH_10_868);
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
  e |= sx1272.setNodeAddress(8);
  Serial.print(F("Setting node address: state "));
  Serial.println(e, DEC);
  
  // Print a success message
  if (e == 0)
    Serial.println(F("SX1272 successfully configured"));
  else
    Serial.println(F("SX1272 initialization failed"));
}


void listenForData(void)
{
  // Receive message
  e = sx1272.receivePacketTimeoutACK(10000);
  if ( e == 0 )
  {
    Serial.print(F("Receive packet with ACK and retries, state "));
    Serial.println(e, DEC);

    for (unsigned int i = 0; i < sx1272.packet_received.length; i++)
    {
      my_packet[i] = (char)sx1272.packet_received.data[i];
    }
    Serial.print(F("Message: "));
    Serial.println(my_packet);
  }
  else {
    Serial.print(F("Receive packet with ACK and retries, state "));
    Serial.println(e, DEC);
  }
}

void pin_ISR() {
  
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    
    // initialize the LCD
    lcd.begin();
    lcd.backlight();
    lcd.clear();
    lcd.print("LAND THE FUCKING    PLANE!");
    lcd.noBacklight();
    lcd.noDisplay();

  } 
  
  if (buttonState == LOW) {

    // turn off the lcd
    lcd.noBacklight();
    lcd.noDisplay();

  } 

  
}

