#include "FastLED.h"

#define NUM_LEDS 280
CRGB leds[NUM_LEDS];


void setup() {
  Serial.begin(9600);
  FastLED.addLeds<APA102, 11, 13, RGB>(leds, NUM_LEDS);
}


//
// The loop
//

long lastMillis = 0;
long loops = 0;

void loop() {
  long currentMillis = millis();
  loops++;
  
  double t = millis() / 1000.0f;

  for ( int i = 0; i < NUM_LEDS; i++) {
    int h = (int)((float)i/NUM_LEDS * 255 + (t*50)) % 255;
    leds[i] = CHSV(h, 200, 70);
  }
 
  FastLED.show();

  // Show FPS
  if(currentMillis - lastMillis > 1000){
    Serial.print("FPS: ");
    Serial.print(loops);
    Serial.println();
    
    lastMillis = currentMillis;
    loops = 0;
  }
}


