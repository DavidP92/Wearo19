#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 pulseSensor;

//Variables for Heart Rate
const byte RATE_AMT = 4;  // Increase for Average Amount
byte rates[RATE_AMT];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMin;
int beatAvg;

//Variables for agentCheck
long samples = 0;         // Counter for Caluculating the Hz or Reading Rate
long block;               //Average IR at power up
long startTime;           //Used to calculate measurement rate
long delta;

//Variable for Infrared 
long IR;

//Timer Variables
unsigned long timer_1;  // Timer -> agentCheck
unsigned long timer_2;  // Timer -> heartCheck
unsigned long timer_3;   // Timer -> temperatureCheck

uint8_t delay_1 = 15;   // agentCheck
uint8_t delay_2 = 50;    // heartCheck
uint8_t delay_3 = 200;   // temperatureCheck

//Setup Variables for Particle Sensor
byte ledBrightness = 0xFF;   //Options: 0=Offto 255=50mA
byte sampleAverage = 4;      //Options: 1,2,4,8,16,32
byte ledMode = 2;            //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
int sampleRate = 400;        //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
int pulseWidth = 411;        //Options: 69, 118, 215, 411
int adcRange = 2048;         //Options: 2048, 4096, 8192, 16384

void setup(){
  pulseSetup();
  timer_1 = millis();
  timer_2 = timer_1;
  timer_3 = timer_2;

  delay(1000);
}

void loop(){
  unsigned long now = millis();
  
  if(now - timer_1 > delay_1){
    agentCheck();
    timer_1 = now;
  }
  if(now - timer_2 > delay_2){
    heartCheck();
    timer_2 = now;
  }
  if(now-timer_3 > delay_3){
    temperatureCheck();
    timer_3 = now;
  }
  else
    Serial.println("Please touch sensor!");
  }

void pulseSetup(){
 Serial.begin(115200);
 if (!pulseSensor.begin(Wire,I2C_SPEED_FAST)){
  Serial.println("Pulse Sensor not detected.... Check wiring or Connection...");
  while (1);
 }
 Serial.println("Pressure on sensor may be needed...");
 
 pulseSensor.setup();                       //Configure sensor with default settings
 pulseSensor.setPulseAmplitudeRed(0x0A);    //Turn Red LED to low to indicate sensor is running
 pulseSensor.setPulseAmplitudeGreen(0);     //Turn off Green LED 
}

void agentCheck(){
  IR = pulseSensor.getIR();
  if(IR > 10000){
    Serial.println("Agent Detected!");
  }
  else
    Serial.println();
}

void temperatureCheck(){

  float temperatureC;
  float temperatureF;
  
  temperatureC = pulseSensor.readTemperature();
  temperatureF = pulseSensor.readTemperatureF();
  
  Serial.print("Temperature (c):  ");
  Serial.print(temperatureC,4);
  Serial.print("          ");
  Serial.print("Temperature (F): ");
  Serial.println(temperatureF,4);
}
void heartCheck(){
  IR = pulseSensor.getIR();
  if(checkForBeat(IR) == true)
  {
    delta = millis() - block;
    block = millis();
    beatsPerMin = 60 / (delta / 1000.0);

    if (beatsPerMin < 255 && beatsPerMin > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMin;
      rateSpot %= RATE_AMT;

      beatAvg = 0;
      for (byte x=0; x < RATE_AMT; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_AMT;
    }
  }
  Serial.print("IR Value  ");
  Serial.print("Beats Per Minute   ");
  Serial.println("Beats Average     ");
  Serial.print(IR);
  Serial.print("      ");
  Serial.print(beatsPerMin);
  Serial.print("      ");
  Serial.println(beatAvg); 
/*
  if (IR < 50000)
    Serial.print("No Finger Present.....");
  Serial.println();
*/
}
