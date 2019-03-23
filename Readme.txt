=============Title=======================
Open-Source and Low-Cost Test Bed for Automated 5G Channel Measurement in mmWave Band

=============Movitation==================
To provide an affordable and open-source test-bed solution for mmWave channel modeling 
covering 24 GHz and 77 GHz bands. 

=============Built with Arduino==========


=============Specification==========
Carrier frequnecy	24GHz		77GHz
Signal bandwidth	15kHz		0.15MHz
Transmit power		20 +/- 1 dBm    9 +/- 2 dBm
Antenna Gain		20 dB		20 dB
H-plane beamwidth	80 deg		15 deg
E-plane beamwidth	12 deg		15 deg

=============Library used================
---24 Ghz module---
Wire.h Adafruit_MCP4725.h stdlib.h

---Base Unit---
<AccelStepper.h> <Adafruit_GFX.h> <Adafruit_SSD1306.h> <SPI.h> 
<RF24.h>(from github) <stdlib.h> <string.h> <TinyGPS++.h>

---Base Station---
<RF24.h> <SPI.h>


===========Electronics BoQ================
RS232+BT x2
Op Amp x2
Arduino nano x2
Arduino Duo x3
GY-4988 x2
nRF24L01 x2
Antenna x3 
OLED x2
ADC+DACx2
Doppler Radar RS3400 x2(77GHz)x2
Doppler Radar KLC-6 x2(24GHz) x2
GPS+RTL x2
Power regulator (YL105) x2
XY42STH34-1504A (High Stepper motor) x2


==================Pin setup================
---24 GHz module---
A0<->KLC6 Output

---Base Unit---
Serial 2 <-> GPS
Pin 50 <-> 4988 stepPin
Pin 51 <-> 4988 DirectionPin
Pin 46 <-> Optical switch Output
Pin 12 <-> OLED_CLK   
Pin 11 <-> OLED_MOSI  
Pin 10 <-> OLED_RESET 
Pin 09 <-> OLED_DC   
Pin 08 <-> OLED_CS   
Pin 13 <-> OLED_Power
Pin 52 <-> NF24L01 CEpin
Pin 53 <-> Nf24L01 CSpin

---Base Station---
Pin 52 <-> NF24L01 CEpin
Pin 53 <-> Nf24L01 CSpin

=============How to use?====================
comply different programs with respect to the corresponding unit.

For 77GHz radar measurement, RS232<->TTL modules are required for communication between Base Unit and 77GHz radar.
Or connect to the PC using RS232 and run the SiversIMA_Controller.m directly from the PC.
Run the BaseStation program and log into the Base Station CLI using arduino IDE or Putty


Using the following command to control the base unit, 77GHz, 24GHz radars.

//          Command                   Code             Respond
//          Force initialization      NNM01#          ”OK”
//          Reset angle               NNM02#          ”OK”
//          Get status                NNM03#           <text>
//          Get coordinates           NNM04#           S dd.dd / E ddd.dd
//          Get current angle         NNM05#            +/- ssss
//          relay                     NNM06<text>#      "Sent"
//          Go to angle               NNM10(+/-)ssss#   ”Go to +/- ssss”
//          Set 24GHz VCO voltage     NNM23VV#          "Set VCO to VV"
//	    Perform Sweeping from     NNM22VVVV#	Measurement data
	    VV to VV
//	    Troubleshooting	      NNM21#		Measurement data

The 77GHz radar command please refer to Sivers IMA RS3400 manual

==================Contribute==================
The Base Station and Base Unit program is programmed by Chiu Chan
The 24GHz radar controlling program is programmed by Ferdi Kurnia
The 77GHz radar controlling program is programmed by Akram Hourani

==================Credits==================
Thanks for the colleagues from RMIT University, AU for the hardware development and 3D printing 

==================License==================
Electronic & Telecommunication department, RMIT University, hereby disclaims all copyright interest in the program "mmWave measurement testbed" written by Chiu Chan, Ferdi Kurnia, Akram Hourani.
