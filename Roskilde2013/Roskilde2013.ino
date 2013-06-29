
#define SPOT_COUNT 5
#define PIN_SPOT_OFFSET 5
#define PIN_SPOT_COUNT 3
#define FLASH 8
#define LASER 9
#define LASER2 10
#define OUTPUT_PINS 11
#define BUTTON 12

#define BEATS_PER_CONTROL 64
#define SHORT_PRESS_TIME 5
#define LONG_PRESS_TIME 500

static int vuPins[] = {0, 1, 2, 3, 4, 6, 7, 8, 9, 10};
static int  vuSize = 10;

// Button mode state
enum ButtonShortPressMode {
  nextMode,
  ambientMode,
  beatMode,
  godMode,
  chaosMode
};
ButtonShortPressMode shortPressMode = nextMode;
int longPresSequence = 0;
long shortPressCount = 0;

boolean pins[OUTPUT_PINS] = {false};

//
// ---------- AUX ---------- 

void allOn(boolean flag) {
    for (int i = 0; i < OUTPUT_PINS; i++) pins[i] = flag;
}

int prng(int x) {
    return ((x * 23413373) % 1337) % 1000;
}

int pRandom(int seed, int from, int to) {
    return (prng(seed) % (to - from)) + from;
}

//
// ---------- INIT ---------- 

void setup() {
    pinMode(BUTTON, INPUT);
    digitalWrite(BUTTON, HIGH);
    for (int i = 0; i < OUTPUT_PINS; i++) {
        pinMode(i, OUTPUT);     
    }
    randomSeed(analogRead(0));
    nextController();
    
    //Serial.begin(9600);                
}

//
// ---------- THE LOOP ---------- 

unsigned long time;
int beat = 0;
int beatDuration = 500;
int beatTime = 0;
int beatRelativeTime = 0;
unsigned long nextBeatTime = 0;
int maxSoundAjustmentInterval = 10;
unsigned long nextmaxSoundAjustment = 0;
void (*topLevelController) ();
unsigned long lastDownPress = 0;
boolean buttonDown = false;
int soundLevel = 0;
float maxSoundLevel = 400;

void setBeat(int newBeat) {
    beat = newBeat;
    beatTime = millis();
}

void loop() {
    // Track time and set the beat
    time = millis();
    beatRelativeTime = time - beatTime;
    if (time > nextBeatTime) {
        nextBeatTime += beatDuration - time + nextBeatTime;
        setBeat(beat + 1);
    }
    
    // Lower the max sound level 1 per 10 ms
    if (time > nextmaxSoundAjustment) {
        nextmaxSoundAjustment += maxSoundAjustmentInterval - time + nextmaxSoundAjustment;
        maxSoundLevel -= 0.1;
    }

    // Read sound level
    soundLevel = abs(analogRead(0) - 512);
    maxSoundLevel = max(max(maxSoundLevel, 300), soundLevel);
    
    // Call a controller
    if(shortPressMode == ambientMode) {
      ambientController();
    } else if (shortPressMode != godMode && shortPressMode != chaosMode) {
        (*topLevelController)();
    }
    
    // Read button value
    boolean buttonDownBefore = buttonDown;
    int buttonValue = digitalRead(BUTTON);
    if (buttonValue == LOW) buttonDown = true;
    else buttonDown = false;
    if (buttonDown && !buttonDownBefore) lastDownPress = time;
    
    // Button release
    if (!buttonDown && buttonDownBefore) {
        if (time - lastDownPress > LONG_PRESS_TIME) {
            longPress();
        } else if (time - lastDownPress > SHORT_PRESS_TIME) {
            shortPress();
        }
    }
    
    // Change pin power
    for (int i = 0; i < OUTPUT_PINS; i++) {
        digitalWrite(i, pins[i]);
    }
}


//
// ---------- Button event handlers ---------- 

unsigned long trackBeatTime = 0;
void shortPress() {
    if (shortPressMode == nextMode) {
        nextController();
        Serial.println("Next mode");
    } else if (shortPressMode == ambientMode) {
        Serial.println("Just chill");
    } else if (shortPressMode == beatMode) {
        Serial.println("Heartbeat");
        beatDuration = time - trackBeatTime;
        trackBeatTime = time;
        setBeat(beat - (beat % 4) + 4);
        nextBeatTime = time + beatDuration;
    } else if (shortPressMode == godMode) {
        Serial.println("Let there be light!");
        for (int i = 0; i < OUTPUT_PINS; i++) pins[i] = shortPressCount % 2 == 0;
        pins[LASER] = false;
    } else if (shortPressMode == chaosMode) {
        Serial.println("When you wish upon a star...");
        allOn(false);
        int pinSpot = random(PIN_SPOT_OFFSET, PIN_SPOT_OFFSET + PIN_SPOT_COUNT);
        pins[pinSpot] = true;
        int spotCount = random(1, 5);
        for (int n = 0; n < spotCount; n++) {
            int pin = random(0, SPOT_COUNT);
            pins[pin] = true;
        }
    }
    longPresSequence = 0;
    shortPressCount++;
}

void longPress() {
    longPresSequence++;
    if (longPresSequence == 1) {
        Serial.println("Next program mode");
        shortPressMode = nextMode;
    } else if (longPresSequence == 3) {
        Serial.println("Ambient mode");
        shortPressMode = ambientMode;
    } else if (longPresSequence == 3) {
        Serial.println("Human beat box mode");
        shortPressMode = beatMode;
    } else if (longPresSequence == 4) {
        Serial.println("GOD mode enabled");
        shortPressMode = godMode;
    } else {
        Serial.println("Mysteries are revealed in the light of no reason");
        shortPressMode = chaosMode;
    }
}

//
// ---------- Top Level controllers ---------- 

void nextController() {
    int n = random(1000);

    if (n < 130) {
      Serial.println("mixController");
      topLevelController = &mixController;
    } else if (n < 260) {
      Serial.println("whiteChillController");
      topLevelController = &whiteChillController;
    } else if (n < 390) {
      Serial.println("lukeSkywalkerController");
      topLevelController = &lukeSkywalkerController;
    } else if (n < 500) {
      Serial.println("crazyShitController");
      topLevelController = &crazyShitController;
    } else if (n < 600) {
      Serial.println("vuController");
      topLevelController = &vuController;
    } else if (n < 700) {
      Serial.println("ballsNLaserController");
      topLevelController = &ballsNLaserController;
    } else if (n < 820) {
      Serial.println("theWaltsController");
      topLevelController = &theWaltsController;
    } else {
      Serial.println("everybodyLovesBenjaminController");
      topLevelController = &everybodyLovesBenjaminController;
    }
      
    allOn(false);
    setBeat(beat % 4);
}

void mixController() {
    if (beat % 24 < 12) {
        spotWalkController();
    } else {
        spotReverseWalkController();
    }
    stroboController(30, 470);
    if (beat > BEATS_PER_CONTROL) nextController();
}

void whiteChillController() {
    pinDualSpotWalkController(4);
    stroboController(30, 5000);
    if (beat > BEATS_PER_CONTROL) nextController();
}

void lukeSkywalkerController() {
    laserController();
    aLittleStroboController();
    if (beat > BEATS_PER_CONTROL) nextController();
}

void crazyShitController() {
    beatStrobo(4);
    for (int i = 0; i < SPOT_COUNT; i++) {
        boolean light = beat % 4 == 0  
            && beatRelativeTime < beatDuration * 0.5 
            && prng(beat) % SPOT_COUNT == i;
        pins[i] = light;
    }         
    if (beat > BEATS_PER_CONTROL * 0.5) nextController();    
}

void vuController() {
    for (int i = 0; i < vuSize; i++) {
        float factor = 0.99 * ((float)i + 1) / vuSize;
        boolean light = soundLevel > factor * maxSoundLevel;
        pins[vuPins[i]] = light;
    }
    if (beat > BEATS_PER_CONTROL) nextController();    
}

void everybodyLovesBenjaminController()  {
    if (beat % 24 < 12) {
        spotWalkController();
        whiteChillController();
    } else {
        spotReverseWalkController();
        stroboController(30, 470);
    }
    if (beat > BEATS_PER_CONTROL) nextController();    
}

void ballsNLaserController() {
    allOn(false);
    laserController();
    pinSpotController();
    if (beat > BEATS_PER_CONTROL) nextController();    
}

void theWaltsController() {
    allOn(false);
    int seed = beat;
    if (beat % 16 >= 12) {
        seed += 1 + (int)(beatRelativeTime / (beatDuration / 3));
    }
    int pinSpot = pRandom(seed, PIN_SPOT_OFFSET, PIN_SPOT_OFFSET + PIN_SPOT_COUNT);
    pins[pinSpot] = true;
    int spotCount = pRandom(seed, 1, 5);
    for (int n = 0; n < spotCount; n++) {
        int pin = pRandom(seed, 0, SPOT_COUNT);
        pins[pin] = true;
    }  
  
    if (beat > BEATS_PER_CONTROL) nextController();    
}

void ambientController() {
    pinDualSpotWalkController(24);
    slowPpotWalkController(16);
    
}

//
// ---------- Spot (MR 16) controllers ---------- 

void spotWalkController() {
    for (int i = 0; i < SPOT_COUNT; i++) {
        pins[i] = beat % SPOT_COUNT == i;
    }    
}

void spotReverseWalkController() {
    for (int i = 0; i < SPOT_COUNT; i++) {
        pins[i] = beat % SPOT_COUNT == SPOT_COUNT - 1 - i;
    }    
}

void slowPpotWalkController(int slowFactor) {
    for (int i = 0; i < SPOT_COUNT; i++) {
        boolean flag = (beat / slowFactor) % SPOT_COUNT == i;
        pins[i] = flag;
    }
}

//
// ---------- Flash (strobo) controllers ---------- 

void stroboController(int onTime, int offTime) {
    int n = time % (onTime + offTime);
    pins[FLASH] = n < onTime;
}

void aLittleStroboController() {
    int onTime = 500;
    int offTime = 8000;
    int n = time % (onTime + offTime);
    if (n < onTime) {
        stroboController(30, 90);
    }
}

void beatStrobo(int slowFactor) {
   int onTime = (prng(beat / slowFactor) % 100) + 30;
   int offTime = onTime;
   stroboController(onTime, offTime);
}


//
// ---------- Pin spot controllers ---------- 

void pinDualSpotWalkController(int slowFactor) {
    for (int i = 0; i < PIN_SPOT_COUNT; i++) {
        boolean flag = (beat / slowFactor) % PIN_SPOT_COUNT == i || (beat / slowFactor + 1) % PIN_SPOT_COUNT == i;
        pins[i + PIN_SPOT_OFFSET] = flag;
    }
}

void pinSpotController() {
    for (int i = 0; i < PIN_SPOT_COUNT; i++) {
        pins[i + PIN_SPOT_OFFSET] = true;
    }
}

//
// ---------- Laser spot controllers ---------- 

void laserController() {
    pins[LASER] = true;
    pins[LASER2] = true;
}












