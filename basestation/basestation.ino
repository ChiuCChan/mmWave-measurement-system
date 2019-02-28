
/*
  Getting Started example sketch for nRF24L01+ radios
  This is a very basic example of how to send data from one node to another
  Updated: Dec 2014 by TMRh20
*/

#include <SPI.h>
#include "RF24.h"


/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
bool radioNumber = 2; // 0, 1for arduino RX; 2 for MATLAB RECEIVER
byte inData;
byte inDaraRadio;


char msg32[32]; //temp

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(52, 53); // (uint8_t _cepin, uint8_t _cspin)
/**********************************************************/

byte addresses[3][6] = {"1Node", "2Node","3Node"};

// Used to control whether this node is sending or receiving

void setup()
{
  Serial.begin(115200);
  Serial.println("Serial to RF24L01");
  Serial.println("akram.hourani@rmit.edu.au");
  Serial.println("");
  Serial.println("Radio Properties:");
  radio.begin();

  // Set the PA Level low to prevent power supply related issues since this is a
  // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_2MBPS);
  radio.printDetails();
  radio.setChannel(125);
  // Set the pipe addressess ** make sure you have the opposite addresses on the SLAM Car
  if (radioNumber==1)  //open pipe for communication
  {
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1, addresses[0]);
    radio.openReadingPipe(2,addresses[2]);
  }
  else if(radioNumber==0)
  {
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1, addresses[1]);
    radio.openReadingPipe(2,addresses[2]);
  }
  else if(radioNumber==2)
  {
    radio.openWritingPipe(addresses[2]);
    radio.openReadingPipe(1, addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }

  Serial.println("");
  Serial.println("Trasmitter ready... ok");
}

void loop()
{
  radio.startListening(); //RX Mode
  if (radio.available())
  {
    memset(msg32, 0, 32);
    radio.read(msg32, 32);
    //Serial.print(msg32);
    Serial.write(msg32, 32);
  }

  if (Serial.available())
  {
    radio.stopListening(); // Swtich to TX mode
    inData = Serial.read();
    radio.write(&inData, sizeof(inData) );
    Serial.write(inData); // For debugging
  }
}

