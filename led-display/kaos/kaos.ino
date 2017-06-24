#include "FastLED.h"

#define NUM_LEDS_X 15
#define NUM_LEDS_Y 18
#define NUM_LEDS 280
CRGB leds[NUM_LEDS];


// Globals 

float a0 = 0.9;
float a1 = 0.9;

float td = 0;
float t = 0;

void setup() {
  Serial.begin(9600);
//  FastLED.addLeds<WS2801, 8, 9, RGB>(leds, NUM_LEDS);
  FastLED.addLeds<APA102, 11, 13, RGB, DATA_RATE_MHZ(4)>(leds, NUM_LEDS);
}


//
// Programs
//

class Program {

    public:
    ~Program(){};

    virtual void reset() = 0;
    virtual void frame(float t) = 0;
    virtual CHSV pixel(float t, float x, float y, int ix, int iy) = 0;

};

class Gray : public Program {

  public:
  
  void reset() {};
  void frame(float t) {}
  
  CHSV pixel(float t, float x, float y, int ix, int iy) {
    //float h = fmod((t) * 0.01, 1);
    return hsv(0, 0, 1);
  }
};

class ColorBall : public Program {

  public:
  
  void reset() {};

  void frame(float t) {
    cx = cos(t) * 0.2;
    cy = sin(t) * 0.2;
  }
  
  CHSV pixel(float t, float x, float y, int ix, int iy) {
    float d = dist2(x, y, cx, cy);
    float v = ball(d);
    float h = fmod((t) * 0.01 + d * 0.3, 1);
    return hsv(h, s * a1, v * a0);
  }

  private:
  float s = 1;
  float cx = 0;
  float cy = 0;
};

class Circle : public Program {

  public:
  
  void reset() {};
  void frame(float t) {}
  
  CHSV pixel(float t, float x, float y, int ix, int iy) {
    float r = abs(sinf(t + a1));
    float h  = fmod(t*0.1 + a0, 1);
    float d = dist2(x, y, 0, 0);
    float v = ball((d - r) * 10);
    return hsv(h, 1, v);
  }
};


class Pong : public Program {

  public:
  
  void reset() {};
  void frame(float t) {
    if(ballPositionX < -1 || ballPositionX > 1) ballVelocityX *= -1;
    if(ballPositionY < -1 || ballPositionY > 1) ballVelocityY *= -1;
    ballPositionX += ballVelocityX * td;
    ballPositionY += ballVelocityY * td;
  }
  
  CHSV pixel(float t, float x, float y, int ix, int iy) {
    // ball
    float d = dist2(x, y, ballPositionX, ballPositionY);
    float v = ball(d * 5);
    float h  = fmod(t*0.1 + a0, 1);
    return hsv(h, a1, v);
        
  }

  private:
  float racket1 = 0.5;
  float racket2 = 0.5;
  float ballPositionX = 0.5;
  float ballPositionY = 0.5;
  float ballVelocityX = 1.0597451;
  float ballVelocityY = 1.94872;
};

//
// Controller
//

// Avaliable programs
Pong pong;
ColorBall colorBall;
Circle circle;
Gray gray;
Program *program = &gray;

void changeProgram(float t) {
  int n = ((int)t/10) % 3;
  if(n == 0) program = &circle;
  else if(n == 1) program = &colorBall;
  else program = &pong;
  program->reset();
}


//
// The loop
//

long lastMillis = 0.5;
long loops = 0.5;

float NUM_LEDS_X_R = 1.0/NUM_LEDS_X;
float NUM_LEDS_Y_R = 1.0/NUM_LEDS_Y;

void loop() {
  long currentMillis = millis();
  loops++;
  
  float newTime = millis() / 1000.0f;
  td = newTime - t; 
  t = newTime;
  changeProgram(t);
  
  a0 = (float)analogRead(A0) / 1023;
  a1 = (float)analogRead(A1) / 1023;

  program->frame(t);
  for ( int i = 0; i < NUM_LEDS; i++) {
    int iy = i / NUM_LEDS_X;
    int ix = i % NUM_LEDS_X;
    if(iy % 2 == 1) ix = NUM_LEDS_X - ix - 1; // Snake layout
    
    // Unit coordinates
    float y = (float)iy / NUM_LEDS_Y * 2 - 1 + NUM_LEDS_Y_R;
    float x = (float)ix / NUM_LEDS_X * 2 - 1 + NUM_LEDS_X_R;

    leds[i] = program->pixel(t, x, y, ix, iy);
  }
 
  FastLED.show();

  // Show FPS
  if(currentMillis - lastMillis > 1000){
    Serial.print("FPS: ");
    Serial.print(loops);
    Serial.print(", a0 ");
    Serial.print(a0);
    Serial.print(", a1 ");
    Serial.print(a1);
    Serial.println();
    
    lastMillis = currentMillis;
    loops = 0;
  }
}





//
// Utilities
//

CHSV hsv(float h, float s, float v) {
  return CHSV(h * 255, s * 255, v * 255);
}

float smoothstep(float edge0, float edge1, float x) {
  // https://en.wikipedia.org/wiki/Smoothstep
  x = constrain((x - edge0) / (edge1 - edge0), 0.0, 1.0);
  return x * x * (3 - 2 * x);
}

float ball(float x) {
  x = abs(x);
  x = constrain(x, 0.0, 1.0);
  x = -x + 1;
  return x * x * (3 - 2 * x);
}

float dist(float a, float b) {
  float d = abs(a - b);
  return min(d, 1 - d);
}

float dist2(float x1, float y1, float x2, float y2) {
  float dx = x1 - x2;
  float dy = y1 - y2;
  return sqrt(dx*dx + dy*dy);
}

float cosOne(float x) {
  return (cos(x * PI) + 1) * 0.5;
}


