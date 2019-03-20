//Library Import For Neopixel Strips and Rings
//Code Modified and Designed by David Paez

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
//Libraries of Particle Sensor
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 pulseSensor;

//Variables for Heart Check
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

//Setup Variables for Particle Sensor
byte ledBrightness = 0xFF;   //Options: 0=Offto 255=50mA
byte sampleAverage = 4;      //Options: 1,2,4,8,16,32
byte ledMode = 2;            //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
int sampleRate = 400;        //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
int pulseWidth = 411;        //Options: 69, 118, 215, 411
int adcRange = 2048;         //Options: 2048, 4096, 8192, 16384

#define bigO A1
#define medO A2
#define lilO 3

#define ledM 16
#define ledB 30
#define ledS 12
#define MBRIGHTNESS 175
#define BBRIGHTNESS 150

#define PIN A0
#define N_LEDS 30

const uint8_t clear_led[4] = {255, 255, 255, 255};
const uint8_t clear_bright = 0;

#define MAXBRIGHTNESS 230
#define MINBRIGHTNESS 0
uint8_t modBrightness = 0;
bool brightnessDir = false;

float temperatureC;
float temperatureF;

Adafruit_NeoPixel strip0 = Adafruit_NeoPixel(ledB, bigO, NEO_RGBW + NEO_KHZ800);
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(ledM, medO, NEO_RGBW + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(ledS, lilO, NEO_RGBW + NEO_KHZ800);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

bool agent = false;
//Timer Variables
unsigned long timer_1;  // Timer -> agentCheck
unsigned long timer_2;  // Timer -> heartCheck
unsigned long timer_3;   // Timer -> temperatureCheck

uint8_t delay_1 = 10000;   // agentCheck
uint8_t delay_2 = 10;    // heartCheck
uint8_t delay_3 = 20000;   // temperatureCheck

void pulseSetup() {
  Serial.begin(115200);
  if (!pulseSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("Pulse Sensor not detected.... Check wiring or Connection...");
    while (1);
  }
  Serial.println("Pressure on sensor may be needed...");


  pulseSensor.setup();                       //Configure sensor with default settings
  pulseSensor.setPulseAmplitudeRed(0x0A);    //Turn Red LED to low to indicate sensor is running
  pulseSensor.setPulseAmplitudeGreen(0);     //Turn off Green LED
}

void agentCheck() {
  IR = pulseSensor.getIR();
  if (IR > 10000) {
    Serial.println("Agent Detected!");
    agent = true;
    return agent;
  }
  else{
    Serial.println("Where the MOFO at!");
    agent = false;
    return agent;
  }
}
void temperatureCheck() {
  temperatureC = pulseSensor.readTemperature();
  temperatureF = pulseSensor.readTemperatureF();

  Serial.print("Temperature (c):  ");
  Serial.print(temperatureC);
  Serial.print("          ");
  Serial.print("Temperature (F): ");
  Serial.println(temperatureF);
  return temperatureC;
  return temperatureF;
}
void heartCheck() {
  IR = pulseSensor.getIR();
  if (checkForBeat(IR) == true)
  {
    delta = millis() - block;
    block = millis();
    beatsPerMin = 60 / (delta / 1000.0);

    if (beatsPerMin < 255 && beatsPerMin > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMin;
      rateSpot %= RATE_AMT;

      beatAvg = 0;
      for (byte x = 0; x < RATE_AMT; x++)
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
  return IR; 
  return beatsPerMin;
  return beatAvg;
  /*
    if (IR < 50000)
      Serial.print("No Finger Present.....");
    Serial.println();
  */
}
void setupPixels(Adafruit_NeoPixel *obj) {
  obj->begin();
  obj->setBrightness(0);
  obj->show();
}

// Fill the dots one after the other with a color
void setPColor(Adafruit_NeoPixel *obj, const uint8_t *ary, const uint8_t brightness) {
  obj->setBrightness(brightness);
  for (uint8_t i = 0; i < obj->numPixels(); i++) {
    obj->setPixelColor(i, obj->Color(ary[0], ary[1], ary[2], ary[3]));
    obj->show();
  }
}

void rainbow(Adafruit_NeoPixel *obj, uint8_t wait) {
  uint16_t i, j;
  for (j = 0; j < 256; j++) {
    for (i = 0; i < obj->numPixels(); i++) {
      obj->setPixelColor(i, Wheel(obj, (i + j) & 255));
    }
    obj->setBrightness(modBrightness);
    obj->show();
    delay(wait);
    dynamicBrightness();
  }
}

void dynamicBrightness() {
  if (!brightnessDir)
    modBrightness++;
  else
    modBrightness--;
  if (modBrightness == MAXBRIGHTNESS || modBrightness == MINBRIGHTNESS)
    brightnessDir = !brightnessDir;
}

void colorselect(byte colorNumber){
  uint8_t a[] = {1, 255, 1, 5};   //Designate Color-------> Red
  uint8_t b[] = {1, 1, 255, 1}; //Designate Color -----> Blue
  uint8_t c[] = {255, 1, 1};
  uint8_t d[] = {1, 1, 255};
  uint8_t e[] = {255, 255, 255};

  switch(colorNumber){
    case 1:               //Red
      setPColor(&strip0, a, 150); 
      setPColor(&strip1, a, 150); 
      setPColor(&strip2, b, 150); 
      setPColor(&strip3, c, 255);
      break;
    case 2:               //Blue
      setPColor(&strip0, b, 50); 
      setPColor(&strip1, b, 50); 
      setPColor(&strip2,b,50);
      setPColor(&strip3,d,255);
      break;
    case 3:                //White   
      setPColor(&strip0, clear_led, 155);
      setPColor(&strip1, clear_led, 155);
      setPColor(&strip3, e, 255);
      break;
    case 4:               //Clearing of Lights 
      setPColor(&strip0, clear_led, clear_bright);  //Turn Off
      setPColor(&strip1, clear_led, clear_bright); // Turn Off
      setPColor(&strip2, clear_led, clear_bright);
      setPColor(&strip3, clear_led, clear_bright);
  }
}
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(Adafruit_NeoPixel *obj, byte WheelPos) {
  if (WheelPos < 85) {
    return obj->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip0.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return obj->Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void setup() {
  setupPixels(&strip0);
  setupPixels(&strip1);
  setupPixels(&strip2);
  setupPixels(&strip3);
  pulseSetup();
  timer_1 = millis();
  timer_2 = timer_1;
  timer_3 = timer_2;
}

void loop() {
  // Some example procedures showing how to display to the pixels:
  unsigned long now = millis();
  
   if(timer_3 - now > delay_3){
    temperatureCheck();    
    Serial.println("[9]");
    timer_2 = now;
  }
  else if(now-timer_3 > delay_3){
    temperatureCheck();
    timer_3 = now;
  } 
  else if(now - timer_2 > delay_2){
    heartCheck();
    if(agent == false){
      Serial.println("[5]");

      delay(500);
      Serial.println("[6]");
      setPColor(&strip0, clear_led, clear_bright);  //Turn Off
      setPColor(&strip1, clear_led, clear_bright); // Turn Off
      setPColor(&strip2, clear_led, clear_bright);
      setPColor(&strip3, clear_led, clear_bright);
    }
    else if (agent == true){
      Serial.println("[7]");
 
      delay(2500);    
      Serial.println("[8]");
      setPColor(&strip0, clear_led, clear_bright);  //Turn Off
      setPColor(&strip1,clear_led,clear_bright);  // Turn Off
      setPColor(&strip2,clear_led,clear_bright);
      setPColor(&strip3,clear_led,clear_bright);
      }
    timer_1 = now;
  }
  else
    Serial.println("Please touch sensor!");
}
