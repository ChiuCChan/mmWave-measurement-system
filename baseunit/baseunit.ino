//
//float val = 0;          // variable to store the value read
//float Voltage;          // Analog voltage
//float Volt[10];         // Analog voltage array
//float averagevolt=0;    // average of the input voltage
//int displaycounter=0;
//#define analogPin 3     // potentiometer wiper (middle terminal) connected to analog pin 
                          // outside leads to ground and +5V


/**************************************************/
#include <AccelStepper.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include "RF24.h"
#include <stdlib.h>
#include <string.h>
#include <TinyGPS++.h>
/**************************************************/
// The TinyGPS++ object
TinyGPSPlus gps;
#define ss Serial2
#define GPSBaud 9600    // GPS have to be 9600Hz 
/**************************************************/
//NEMA 17 motor pins
#define DRIVER 1
#define stepPin 50
#define dirPin 51       // initialize the stepper library on pins 50 51: 51=step 50=direction
AccelStepper myStepper(DRIVER,stepPin, dirPin);
/**************************************************/
//Optical Switch pin
#define opticalsw 46
/**************************************************/
// OLED screen pins
#define OLED_CLK   12
#define OLED_MOSI  11
#define OLED_RESET 10
#define OLED_DC    9
#define OLED_CS    8
#define OLED_Power 13
#define Buzzer 3
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
/**************************************************/
//Radio pin
#define CEpin 52
#define CSpin 53

/***********************************************************/
/***      Set this radio as radio number 0 or 1         ***/
char radiodisplay[32];
int radioNumber = 1; // 0, 1for arduino RX; 2 for MATLAB RECEIVER
byte incomingByte;     // When byte is received from the Radio
short int Buffer_index = 0;
char inData[52];       //Serial data buffer
char Command[52];      //The command received from serial port
double latdeg = 0;     //GPS latitude degree
double longdeg = 0;    //GPS longitude degree
bool error = 0;       //for checking command error
double angle=0;       //used to record current angle
String serialbuffer;
char charbuffer[1000];
char buffershort[32];
/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(CEpin, CSpin); // (uint8_t _cepin, uint8_t _cspin)
/**********************************************************/
//byte addresses[3][6] = {"1Node", "2Node","3Node"};

byte Node1[2][7] = {"1Node", "2Node"};
byte Node2[2][7] = {"3Node", "4Node"};

// Used to control whether this node is sending or receiving

// Our function
void beep(unsigned char delayms) 
 { //creating function
  digitalWrite(Buzzer, LOW); //Setting pin to low
  delay(delayms); //Delaying
  digitalWrite(Buzzer ,HIGH); //Setting pin to high
  delay(delayms); //Delaying;r
 }
 
void moveto()
{
 char Stepc[4];
 int Stepint = 0;

 memset(Stepc,0,sizeof(Stepc));
              for (int i = 0 ; i < 4; i++) 
              {
                Stepc[i] = Command[i + 7]; //copy  digits 7,8,9,10 step
              }
              if (strstr(Command,"+"))
              {
              //  display.println("debug1");
                Stepint=atoi(Stepc);
              }
              else if (strstr(Command,"-"))
              {
                //display.println("debug2");
                Stepint= -atoi(Stepc);
              }
              display.print("Move to position");
              display.println(Stepint); 
              Serial.println(Stepc);
              display.display();
              radio.write("N2 moved to position:",21);
              radio.write(Stepc,sizeof(Stepc));
              radio.write("\r",2);
              myStepper.setCurrentPosition(0);      
              myStepper.runToNewPosition(Stepint); // Move steps (1.8 degree for one step)
              angle+=(Stepint*1.8)/16;
              angle=angle/360;
         
              delay(2000);
}

void getstatus(double currentangle)
{
  radio.write("Current Angle:\r",15);
  memset(radiodisplay,0,32);
  snprintf(radiodisplay,32,"%f",currentangle);
  radio.write(&radiodisplay, sizeof(radiodisplay));
  radio.write("\r",2);
}
void relay()
{
  char msg32[32];
  memset(msg32,0,sizeof(msg32));
  for(int i=0; Command[i + 6] != '#' ;i++)
  {
    msg32[i] = Command[i + 6]; //copy  digits 6,8,9,10 step
  }
  
  serialbuffer =String(msg32);
  
  Serial1.print(serialbuffer + "\r");
  Serial.println(serialbuffer);        //for debug
  display.println(serialbuffer);
  display.display();
  delay(100);
}


void relay22()
{
  char msg32[32];
  memset(msg32,0,sizeof(msg32));
  int i=0;
  for(i=0; Command[i + 6] != '#' ;i++)
  {
    msg32[i] = Command[i + 6]; //copy  digits 6,8,9,10 step
  }
  msg32[i] = '#';
  serialbuffer =String(msg32);
  
  Serial3.print(serialbuffer);
  Serial.println(serialbuffer);        //for debug
  display.println(serialbuffer);
  display.display();
  //delay(100);
}

void calibrate()
{
  radio.write("Start Calibrate\r",17);
  for(int i=0;i<360;i++)
  {
      myStepper.setCurrentPosition(0);      
      myStepper.runToNewPosition(8.888); // Move one step (1.8 degree for one step)
      if (digitalRead(opticalsw) == LOW)       //Try to get "01" signal to calibrate
      {
          myStepper.setCurrentPosition(0);      
          myStepper.runToNewPosition(8.888); // Move one step (1.8 degree for one step)
          if (digitalRead(opticalsw) == HIGH)
          {
              myStepper.setCurrentPosition(0);
              myStepper.runToNewPosition(8.888); // Move one step (1.8 degree for one step)
              radio.write("OK\r",4);
              display.println("Finish Calibration!");
              display.display();
              return;
          }   
      }
  }
}

void GPSget(double latitude,double longitude)     //GPS function for getting lattude & longitude
{
  latitude = gps.location.lat();
  delay(100);
  longitude = gps.location.lng();
  delay(100);
  radio.write("S:",2);
  delay(100);
  snprintf(radiodisplay,50,"%f",latitude);
  radio.write(&radiodisplay,sizeof(latitude));
  delay(100);
  radio.write("E:",2);
  delay(100);
  memset(radiodisplay, 0, 32);
  snprintf(radiodisplay,50,"%f",longitude);
  radio.write(&radiodisplay,sizeof(longitude));
  delay(100);
  radio.write("\r",sizeof("\r"));
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS data received: check wiring"));  //for debugging
    display.clearDisplay();  ///LED module
    display.setCursor(0, 0);
    display.println("No GPS data received");
    display.print("Lat:");
    display.println(latitude);
    display.print("Long:");
    display.println(longitude);
    display.display();      ///LED module
  }
  else
  {
    display.clearDisplay();  ///LED module
    display.setCursor(0, 0);
    display.print("Lat:");
    display.println(latitude);
    display.print("Long:");
    display.println(longitude);
    display.display();      ///LED module
  }
}

void setup()
{
  pinMode(opticalsw, INPUT);     //  Digital input from 
  pinMode(Buzzer, OUTPUT); //Set buzzerPin as output
  digitalWrite(Buzzer ,HIGH); //Setting pin to high
  
  // This is power up the OLED from the output pin
  pinMode(OLED_Power, OUTPUT);
  digitalWrite(OLED_Power, HIGH);
  
  //initialise stepper motor
  myStepper.setMaxSpeed(500.0);
  myStepper.setAcceleration(5000.0);
  myStepper.setSpeed(500);

   // Initilize GPS
  ss.begin(GPSBaud);
  // Initilize Relay module
  Serial1.begin(115200);
  Serial3.begin(115200);
  // Initilize OLED
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("mmWave demoV6");
  display.println("akram.hourani");
  display.println("@rmit.edu.au");
  display.println("20170504");
  display.display();
  delay(500);
 
  //  setup serial
  Serial.begin(115200);
  Serial.println("mmWave motherbroad");
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
  if (radioNumber==2)  //open pipe for communication
  {
    radio.openReadingPipe(1,Node2[0]);
    radio.openWritingPipe(Node2[1]);
    radio.stopListening();
    radio.setChannel(75);
    radio.write("Node2 online",13);
  }
  else if(radioNumber==1)
  {
    radio.openReadingPipe(1,Node1[0]);
    radio.openWritingPipe(Node1[1]);
    radio.stopListening();
    radio.setChannel(125);
    radio.write("Node1 online",13);
  }
  Serial.println("");
  Serial.println("Trasmitter ready... ok");
}

void loop()
{
  radio.startListening(); //RX Mode
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Listening to Radio");
  display.display();
  if (radio.available()) 
  {
      radio.read( &incomingByte, sizeof(incomingByte) );
      inData[Buffer_index] = incomingByte;
      Buffer_index++;
      radio.stopListening();
      
      //When a commdan is ready execute the following
      if (inData[Buffer_index - 2] == '#' && Buffer_index > 1)  
      {
        memset(Command, 0, sizeof(Command)); // Clear recieved buffer
        strncpy(Command, inData, Buffer_index); // Save the command
        memset(inData, 0,sizeof(inData));
        Serial.print(inData);  
       
        Buffer_index = 0; // reset the buffer pointer
 
        display.clearDisplay();  ///LED module /for debugging
        display.setCursor(0, 0);
        display.print("Command: ");
        display.println(Command);
        display.display();      ///LED module

//          Command                   Code             Respond
//          Force initialization      NNM01#          ”OK”
//          Reset angle               NNM02#          ”OK”
//          Get status                NNM03#           <text>
//          Get coordinates           NNM04#           S dd.dd / E ddd.dd
//          Get current angle         NNM05#            +/- ssss
//          relay                     NNM06<text>#      "Sent"
//          Go to angle               NNM10(+/-)ssss#   ”Go to +/- ssss”
        
            if(strstr(Command,"N1M01") || strstr(Command,"N2M01"))
            {
//              initialization();
            }
            else if(strstr(Command,"N1M02") || strstr(Command,"N2M02") )
            {
              calibrate();
              angle=0;
            }
            else if(strstr(Command,"N1M03") || strstr(Command,"N2M03"))
            {
              getstatus(angle);
            }
            else if(strstr(Command,"N1M04") || strstr(Command,"N2M04"))
            {
              GPSget(latdeg,longdeg); 
            }
            else if(strstr(Command,"N1M06") || strstr(Command,"N2M06"))
            {
              relay();
            }
            else if(strstr(Command,"N1M07") || strstr(Command,"N2M07"))
            {
              relay22();
            }
            else if(strstr(Command,"N1M10") || strstr(Command,"N2M10"))
            {
              moveto();  
            }
      }
  }
  while(Serial1.available())
  {
    radio.stopListening();
    memset(buffershort,'\0',sizeof(buffershort));
    serialbuffer = Serial1.readBytes(buffershort,30); // reading 32 bytes  
    Serial.println(buffershort);
    radio.write(buffershort,30);
  }
  while(Serial3.available())
  {
    radio.stopListening();
    memset(buffershort,'\0',sizeof(buffershort));
    Serial3.readBytes(buffershort,32); // reading 32 bytes  
    Serial.print(buffershort);
    radio.write(buffershort,32);
  }
  
  
  //delay(20);

}
