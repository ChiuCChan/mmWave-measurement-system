#include <Wire.h>
#include <Adafruit_MCP4725.h>
#include <stdlib.h>

#define voltsIn A0

Adafruit_MCP4725 dac; // constructor

/////////////////////////////////////////////////////////////
char inData[52]; //Serial data buffer
short int Buffer_index = 0;
char Command[52];      //The command received from serial port
byte incomingByte;
char conc_val[52];

#define n_sample_1 100
float volt_array_1[n_sample_1];
float E = 0.0;
float  volt_avg, volt_max_temp;

int delay_val = 1;//delay between sample
int delay_voltage = 100;//delay between Setting up VCO voltage to taking first ADC value
    
/////////////////////////////////////////////////////////////

int x = 0;
String inputread, endchar;
char val; // Data received from the serial port

int getIndexOfMaximumValue(float* array, int size)
{
  int maxIndex = 0;
  float max = array[maxIndex];
  for (int i=0; i<size; i++)
  {
    if (max < array[i])
    {
      max = array[i];
      maxIndex = i;
    }
  }
  return maxIndex;
}

void setup(void) 
{
  Serial.begin(115200);
  dac.begin(0x60); // The I2C Address: Run the I2C Scanner if you're not sure      
  Serial.println("24 GHz Front-End");
}

void start_measuring()
{
    int adcValueRead = 0;
    float voltageRead = 0;
       
    inputread = "";
    
    for(int i = 0 ; i < 100 ; i++)
    {
        adcValueRead = analogRead(voltsIn);
        voltageRead = (adcValueRead * 5.0)/ 1024.0;
           
           if(i == 99)
            {
              endchar =  '#';
              inputread = inputread + voltageRead + endchar;
            } 
            
            if (i > 0 && i < 99)
            {
              endchar =  ", ";
              inputread = inputread + voltageRead + endchar;
            } 
            
            if (i == 0)
            {
              endchar =  ", ";
              inputread = voltageRead + endchar;
            }
            //inputread = inputread + voltageRead + endchar;
     
      }
    
      Serial.print(inputread);
      Serial.println();
}


void set_dac_voltage(char* dac_volt)
{
    uint32_t dac_value;
    float dac_expected_output;
      
    String inString1 = "";
    String inString2 = "";
    String inString3 = "";
    double val_volt, val_volt2, val_volt3;
    int v1, v2, v3;

    inString1 = Command[4];
    inString2 = Command[5];
    inString3 = Command[6];
    v1 = inString1.toInt();
    v2 = inString2.toInt();
    v3 = inString3.toInt();
              
    val_volt = (v1 * 100.0 + v2 * 10.0 + v3);
    val_volt2 = val_volt/100.0;
    Serial.println(v1);
    Serial.println(v2);
    Serial.println(v3);
    Serial.println(val_volt2);
    
    dac_value = ((val_volt2/2.0)/5.0*4096.0);//this is for single voltage value assignment
    
    dac_expected_output = ((val_volt2*2.0)/4096.0) * dac_value;
    dac.setVoltage(dac_value, false);
   
    Serial.println("Set DAC Voltage Success");
    
}

//void start_sweeping(char* Command)
void start_sweeping()
{
    uint32_t dac_value, dac_value1 = 0, dac_value2 = 0, dac_value_set;
    int adcValueRead = 0;
    float voltageRead = 0;
    float dac_expected_output, dac_start, dac_stop, volt_1;
    int val_s = 0, a = 0, N, pwmVolt, maxIndex, maxIndex_final, val_max[52];//204 to 255, variable to store the read value
    float volt_max_temp, max_array[52], dacv_array[52], dacv1_array[52]; 
    float sum = 0;
    
//    int n_sample = 50;
//    float volt_array[n_sample];
//    int val_array[n_sample];//0 to 1023

//    String inString1 = "";
//    String inString2 = "";
//    String inString3 = "";
//    String inString4 = "";
//    
//    double val_volt, val_volt2, val_volt3, val_volt4;
//    int v1, v2, v3, v4;
//
//    inString1 = Command[3];
//    inString2 = Command[4];
//    inString3 = Command[5];
//    inString4 = Command[6];
//    v1 = inString1.toInt();
//    v2 = inString2.toInt();
//    v3 = inString3.toInt();
//    v4 = inString4.toInt();
//              
//    val_volt = (v1 * 10.0 + v2);
//    val_volt2 = val_volt/10.0;
//    Serial.println(v1);
//    Serial.println(v2);
//    Serial.println(val_volt2);
//
//    val_volt3 = (v3 * 10.0 + v4);
//    val_volt4 = val_volt3/10.0;
//    Serial.println(v3);
//    Serial.println(v4);
//    Serial.println(val_volt4);
    
    //Calibrated Values
    //7.5 (DAC value) ==> 7.61 (DCV read)
    //7.4 ==> 7.48
    //7.35 ==> 7.44
    //7.3 ==> 7.39
    //7.25 ==> 7.33
    //7.2 ==> 7.28
    //7.1 ==> 7.19 

    //dac_start = val_volt2;//7.4
    //dac_stop = val_volt4;//7.6

    dac_start = 7.2;
    dac_stop = 7.5;
    
    dac_value1 = ((dac_start/2.0)/5.0*4096.0);//2990 (7.3)
    dac_value2 = ((dac_stop/2.0)/5.0*4096.0);//3112 (7.6)

    Serial.print("Start Sweeping from ");
    Serial.print(dac_start);
    Serial.print(" V to ");
    Serial.print(dac_stop);
    Serial.print(" V");

    N = (dac_value2 - dac_value1)/5;
    
    for (dac_value = dac_value1; dac_value < dac_value2; dac_value = dac_value + 5) //this is for voltage sweeping
    {
      sum = 0;
      E = 0.0;

      //1 Set the DAC Value
      dac.setVoltage(dac_value, false);
      
      delay(delay_voltage);//delay between the voltage
      
      //2 Capture received voltage
      
      for(int i = 0 ; i < n_sample_1 ; i++) 
      { 
        val_s             = analogRead(voltsIn); //read the voltage from A0
        volt_1            = val_s*5.0;
        volt_array_1[i]   = volt_1/1023.0;
        
        delay(delay_val);//delay is between the sample
        
//        Serial.println(" ");
//        int j = i + 1;
//        Serial.print(j);    
//        Serial.print("\t");
//        Serial.print(volt_array_1[i] ,6);
        sum += volt_array_1[i] ;
      }//close bracket for
       
      //3 Get average voltage
      volt_avg = sum/n_sample_1;
     dac_expected_output = ((5.0*2.0)/4096.0) * dac_value;
//      Serial.println(" ");
//      Serial.print("Average[");
//      Serial.print(dac_expected_output);
//      Serial.print("] = ");
//      Serial.print(volt_avg,6);
     
      //4 Calculate energy
      for(int i = 0 ; i < n_sample_1 ; i++) 
      { 
        float temp;
        temp = volt_array_1[i];
        volt_array_1[i] = (temp - volt_avg)*1000.0;
//        Serial.println(" ");
//        Serial.print("Minus avg : ");
//        Serial.print(volt_array_1[i],6);
//        Serial.print(" mV");

        temp = 0;
        temp = volt_array_1[i] * volt_array_1[i];
        E = E + temp;
//        Serial.println(" ");
//        Serial.print("Squared : ");
//        Serial.print(temp,6);
        
      }//close bracket for

//      Serial.println(" ");
//      Serial.println("Energy is ");  
//      Serial.print(E,9);

      max_array[a] = E;
      dacv1_array[a] = dac_value;
      dacv_array[a] = dac_expected_output;
      a++;

      } //end of for 

     
      //5 determine DAC Voltage that causes maximum energy
      Serial.println(" ");
      for(int i = 0 ; i < N ; i++) 
      { 
//            Serial.println(" ");
//            Serial.print("Average[");
//            Serial.print(dacv_array[i]);
//            Serial.print("][");
//            Serial.print(dacv1_array[i]);
//            Serial.print("] = ");
            Serial.println(max_array[i],6);
      }

//     maxIndex = getIndexOfMaximumValue(max_array, N); 
//      Serial.println(" ");
//      //Serial.println(maxIndex);
//      Serial.print("Maximum at ");
//      Serial.print(dacv_array[maxIndex]);
//      Serial.print("V when delay value is ");
//      Serial.print(delay_val);
//      Serial.print(" ms = ");
//      Serial.print(max_array[maxIndex],6);
//      Serial.print(" uW ");

      //6 Set the DAC voltage --commented, beacuse since we are sweeping, we dont need fixed VCO value
//      Serial.println(" ");
//      Serial.println("We will set the DAC Voltage into this value");
//
//      
//      dac_value_set = ((dacv_array[maxIndex]/2.0)/5.0*4096.0);//this is for single voltage value assignment
//      dac.setVoltage(dac_value_set, false);
//      
////      Serial.println(dac_value_set);
////      Serial.println(" ");
////      Serial.println(dacv1_array[maxIndex]);
//      Serial.println(" ");
//      Serial.println("Set DAC Voltage Success");
      
}

void start_sweeping_one_voltage(char* Command)
{
    uint32_t dac_value, dac_value1 = 0, dac_value2 = 0, dac_value_set;
    int adcValueRead = 0;
    float voltageRead = 0;
    float dac_expected_output, dac_start, dac_stop;
    int val_s = 0, a = 0, N, pwmVolt, maxIndex, maxIndex_final;         // variable to store the read value 
    float volt_1;
    
    int val_max[52];//204 to 255
   
    float max_array[52], dacv_array[52]; 

    String inString1 = "";
    String inString2 = "";
    double val_volt, val_volt2;
    int v1, v2;

    double sum = 0;

    inString1 = Command[3];
    inString2 = Command[4];
    v1 = inString1.toInt();
    v2 = inString2.toInt();
              
    val_volt = (v1 * 10.0 + v2);
    val_volt2 = val_volt/10.0;
    
    //dac_value = val_volt2;//7.4
    dac_value = 7.4;
    
    Serial.print("DAC Value 1 : ");
    Serial.print(dac_value);
    Serial.println(" ");
    
    dac.setVoltage(dac_value, false);

    while(1)
    {
      //2 Capture received power
      sum = 0;
      for(int i = 0 ; i < n_sample_1 ; i++) 
      { 
        val_s         = analogRead(voltsIn); //read the voltage from A0
        volt_1        = val_s*5.0;
        volt_array_1[i] = volt_1/1023.0;
        
        delay(delay_val);//delay is between the sample
        
        //Serial.println(" ");
//        int j = i + 1;
//        Serial.print(j);    
//        Serial.print("\t");
//        Serial.print(volt_array_1[i] ,6);
        sum += volt_array_1[i] ;
      }//close bracket for
       
      //delay(50);//delay is between the sample
     
       volt_avg = sum/n_sample_1;
//      Serial.println(" ");
//      Serial.print("Average[");
//      Serial.print(dac_value);
//      Serial.print("] = ");
//      Serial.print(volt_max,6);

      //square
      E = 0.0;
      
      for(int i = 0 ; i < n_sample_1 ; i++) 
      { 
        volt_array_1[i] = volt_array_1[i] - volt_avg;
        volt_array_1[i] = volt_array_1[i]*volt_array_1[i];
        E = E + volt_array_1[i];
      }//close bracket for
     
      Serial.print("Energy is ");  
      Serial.println(E);
    }
     
}

// LIST OF COMMANDS
// M21 : Start Measuring at fix VCO Voltage
// M22 : Start sweeping at range of VCO Values (currently set to 7.2 V to 7.5 V)
// M23 : Set VCO to fix value
// M24 : Energy detection, loop forever


void loop(void) 
{  
    if (Serial.available() > 0) 
    {
      
      //Command stores the received user-inputted command
      incomingByte = Serial.read();
      inData[Buffer_index] = incomingByte;
      Buffer_index++;

      if (inData[Buffer_index - 1 ] == '#' && Buffer_index > 1)
      {
          memset(Command,0,sizeof(Command)); // Clear received buffer
          strncpy(Command, inData, Buffer_index); //Save the command
          Buffer_index = 0; //reset the buffer pointer
          Serial.println(Command);
      
          if (strstr(Command, "M21")) 
          {
              Serial.println("Start Measuring");
              start_measuring(); 
          } else if (strstr(Command, "M22")) 
          {
              //Serial.println("Start Sweeping");
              //start_sweeping(Command); 
              start_sweeping(); 
          } else if (strstr(Command, "M23")) 
          {
              Serial.println("Set Voltage");
              set_dac_voltage(Command); 
          } else if (strstr(Command, "M24")) 
          {
              Serial.println("Set Voltage");
              start_sweeping_one_voltage(Command);
          }  else
          {
              Serial.write(incomingByte);
          }
      }
  }
}


