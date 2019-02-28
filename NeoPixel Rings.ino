#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
 
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

Adafruit_NeoPixel strip0 = Adafruit_NeoPixel(ledB, bigO, NEO_RGBW + NEO_KHZ800);
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(ledM, medO, NEO_RGBW + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(ledS, lilO, NEO_RGBW + NEO_KHZ800);
Adafruit_NeoPixel strip3 = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

 
void setup() {
  setupPixels(&strip0);
  setupPixels(&strip1);
  setupPixels(&strip2);
  setupPixels(&strip3);
  Serial.begin(9600);
}
 
void loop() {
  // Some example procedures showing how to display to the pixels:
  uint8_t a[] = {1, 255, 1, 5};   //Designate Color-------> Red
  uint8_t b[] = {1,1,255,1}; //Designate Color -----> Blue
  uint8_t c[] = {255,1,1};
  uint8_t d[] = {1,1,255};
  uint8_t e[] = {255,255,255};
  delay(50);

  Serial.println("[1]");
  setPColor(&strip0, a, 75); setPColor(&strip1, b, 75); setPColor(&strip2,c,75); setPColor(&strip3,c,175);
  delay(2500);
  
  Serial.println("[2]");
  setPColor(&strip0, clear_led, clear_bright);  //Turn Off
  setPColor(&strip1,clear_led,clear_bright);  // Turn Off
  setPColor(&strip2,clear_led,clear_bright);
  setPColor(&strip3,clear_led,clear_bright);

  Serial.println("[3]");
  setPColor(&strip0, b, 50); setPColor(&strip1, a, 50); setPColor(&strip2,a,50);setPColor(&strip3,d,255);
  delay(2500);  
  
  Serial.println("[4]");
  setPColor(&strip0, clear_led, clear_bright);  //Turn Off
  setPColor(&strip1,clear_led,clear_bright);  // Turn Off
  setPColor(&strip2,clear_led,clear_bright);
  setPColor(&strip3,clear_led,clear_bright);

  Serial.println("[5]");
  setPColor(&strip0, a, 150); setPColor(&strip1, a, 150); setPColor(&strip2,b,150);setPColor(&strip3,c,255);
  delay(2500);  
  
  Serial.println("[6]");
  setPColor(&strip0, clear_led, clear_bright);  //Turn Off
  setPColor(&strip1,clear_led,clear_bright);  // Turn Off
  setPColor(&strip2,clear_led,clear_bright);
  setPColor(&strip3,clear_led,clear_bright);

  Serial.println("[7]");
  setPColor(&strip0, b, 50); setPColor(&strip1, b, 50); setPColor(&strip2,b,50);setPColor(&strip3,d,255);
  delay(2500);  
  
  Serial.println("[8]");
  setPColor(&strip0, clear_led, clear_bright);  //Turn Off
  setPColor(&strip1,clear_led,clear_bright);  // Turn Off
  setPColor(&strip2,clear_led,clear_bright);
  setPColor(&strip3,clear_led,clear_bright);
  
  Serial.println("[9]");
  setPColor(&strip0,clear_led,155);
  setPColor(&strip1,clear_led,155);
  setPColor(&strip3,e,255);
  delay(2000);
    
  setPColor(&strip0, clear_led, clear_bright);  //Turn Off
  setPColor(&strip1,clear_led,clear_bright);  // Turn Off
  setPColor(&strip2,clear_led,clear_bright);
  setPColor(&strip3,clear_led,clear_bright);
  
  Serial.println("[10]");
  rainbow(&strip1, 10);
  rainbow(&strip0, 15);
  rainbow(&strip2,10);
  rainbow(&strip3,10);
  delay(1000);
}


void setupPixels(Adafruit_NeoPixel *obj) {
  obj->begin();
  obj->setBrightness(0);
  obj->show();
}
 
// Fill the dots one after the other with a color
void setPColor(Adafruit_NeoPixel *obj, const uint8_t *ary, const uint8_t brightness) {
  obj->setBrightness(brightness);
  for(uint8_t i=0; i < obj->numPixels(); i++) {
      obj->setPixelColor(i, obj->Color(ary[0], ary[1], ary[2], ary[3]));
      obj->show();
  }
}
 
void rainbow(Adafruit_NeoPixel *obj, uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i < obj->numPixels(); i++) {
      obj->setPixelColor(i, Wheel(obj, (i+j) & 255));
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

 
// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(Adafruit_NeoPixel *obj, byte WheelPos) {
  if(WheelPos < 85) {
   return obj->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip0.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return obj->Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
