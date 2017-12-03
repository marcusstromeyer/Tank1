/*
 SCP1000 Barometric Pressure Sensor Display

 Shows the output of a Barometric Pressure Sensor on a
 Uses the SPI library. For details on the sensor, see:
 http://www.sparkfun.com/commerce/product_info.php?products_id=8161
 http://www.vti.fi/en/support/obsolete_products/pressure_sensors/

 This sketch adapted from Nathan Seidle's SCP1000 example for PIC:
 http://www.sparkfun.com/datasheets/Sensors/SCP1000-Testing.zip

 Circuit:
 SCP1000 sensor attached to pins 6, 7, 10 - 13:
 DRDY: pin 6
 CSB: pin 7
 MOSI: pin 11
 MISO: pin 12
 SCK: pin 13

 created 31 July 2010
 modified 14 August 2010
 by Tom Igoe
 */

// the sensor communicates using SPI, so include the library:
#include <SPI.h>
int slaveSelect=A5; 
int LORA_RESET_PIN=A4;
int x=0;

void setup() {
  Serial.begin(9600);

  pinMode(slaveSelect, OUTPUT);      // sets the digital pin as output
  pinMode(LORA_RESET_PIN, OUTPUT);
  digitalWrite(LORA_RESET_PIN, HIGH);
  delay(100);
  
  digitalWrite(LORA_RESET_PIN, LOW);
  delay(100);

  // start the SPI library:
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV64);
  SPI.setDataMode(SPI_MODE0);

  writeRegister(0x0,0x0);
  writeRegister(0x1,0x81);


  // give the sensor time to set up:
  delay(100);
}

void loop() {
  //Select High Resolution Mode
  delay(1000);

  writeRegister(0x2,0x1A);
  
  delay(1000);

  readRegister(0x2);
  delay(100);
  readRegister(0x1);
}

//Read from or write to register from the SCP1000:
void writeRegister(byte address, byte data)
{
    Serial.println("Inside Write Register");
    //Utils.setCSSocket1();
    bitSet(address, 0);      // Bit 7 set to read from registers
    Serial.println(address);
    delay(1000);
    SPI.transfer(address);
    SPI.transfer(data);
    //Utils.unsetCSSocket1();

    Serial.print(F("## Writing:  ##\t"));
    Serial.print(F("Register "));
    bitClear(address, 7);
    Serial.print(address, HEX);
    Serial.print(F(":  "));
    Serial.print(data, HEX);
    Serial.println();
    
    //Serial.println("Inside Write Register - end");

}

//Sends a write command to SCP1000
void readRegister(byte address)
{
    byte value = 0x00;

    //Utils.setCSSocket1();
    bitClear(address, 7);    // Bit 7 cleared to write in registers
    SPI.transfer(address);
    value = SPI.transfer(address);
    //Utils.unsetCSSocket1();

    Serial.print(F("## Reading:  ##\t"));
    Serial.print(F("Register "));
    Serial.print(address, HEX);
    Serial.print(F(":  "));
    Serial.print(value, HEX);
    Serial.println();

}

