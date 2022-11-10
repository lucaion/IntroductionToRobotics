// declaring all the joystick pins
const int pinSW = 2; 
const int pinX = A0; 
const int pinY = A1;

// declaring all the segments pins
const int pinA = 4;
const int pinB = 5;
const int pinC = 6;
const int pinD = 7;
const int pinE = 8;
const int pinF = 9;
const int pinG = 10;
const int pinDP = 11;

bool commonAnode = false; 
byte state = HIGH;

const int segSize = 8;

const int pinAIndex = 0;
const int pinBIndex = 1;
const int pinCIndex = 2;
const int pinDIndex = 3;
const int pinEIndex = 4;
const int pinFIndex = 5;
const int pinGIndex = 6;
const int pinDPIndex = 7;

int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

byte segmentState[segSize] = {
  LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW
};

byte currentSegmentState[segSize] = {
  LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW
};


// joystick variables
byte swState = LOW;
int xValue = 0;
int yValue = 0;

// variables for joystick moves
bool joyMoved = false;
const int minThreshold = 200;
const int maxThreshold = 800;
const int minDiagonalThreshold = 400;
const int maxDiagonalThreshold = 600;

byte currentSegment = 7;
unsigned long lastBlink = 0;
const int blinkDelay = 300; 
byte currentState = 2;

const int debounceDelay = 50;
unsigned long int lastDebounceTime = 0;
const int longPressThreshold = 2000;
volatile byte lastReading = LOW;
unsigned long pressedTime  = 0;
bool isPressing = false;


void displaySegments(int segments[]) {
  for (int i = 0; i < segSize; i++) {
    digitalWrite(segments[i], segmentState[i]);
  }
}

void resetDisplay() {
  for (int i = 0; i < segSize; i++) {
    segmentState[i] = LOW;
    currentSegmentState[i] = LOW;
  }

  currentSegment = pinDPIndex;
}

void blinkLed(byte currentSegment) {
  if (millis() - lastBlink >= blinkDelay) {
    lastBlink = millis();
    segmentState[currentSegment] = !segmentState[currentSegment];
  }
}

void moveSegment(String direction) {
  if (direction == "left") {
    if (currentSegment == pinAIndex) {
      currentSegment = pinFIndex;
      segmentState[pinAIndex] = currentSegmentState[pinAIndex];
      return;
    }

    if (currentSegment == pinBIndex) {
      currentSegment = pinFIndex;
      segmentState[pinBIndex] = currentSegmentState[pinBIndex];
      return;
    }

    if (currentSegment == pinCIndex) {
      currentSegment = pinEIndex;
      segmentState[pinCIndex] = currentSegmentState[pinCIndex];
      return;
    }

    if (currentSegment == pinDIndex) {
      currentSegment = pinEIndex;
      segmentState[pinDIndex] = currentSegmentState[pinDIndex];
      return;
    }

    if (currentSegment == pinDPIndex) {
      currentSegment = pinCIndex;
      segmentState[pinDPIndex] = currentSegmentState[pinDPIndex];
      return;
    }
  }

  if (direction == "right") {
    if (currentSegment == pinAIndex) {
      currentSegment = pinBIndex;
      segmentState[pinAIndex] = currentSegmentState[pinAIndex];
      return;
    }

    if (currentSegment == pinCIndex) {
      currentSegment = pinDPIndex;
      segmentState[pinCIndex] = currentSegmentState[pinCIndex];
      return;
    }

    if (currentSegment == pinDIndex) {
      currentSegment = pinCIndex;
      segmentState[pinDIndex] = currentSegmentState[pinDIndex];
      return;
    }

    if (currentSegment == pinEIndex) {
      currentSegment = pinCIndex;
      segmentState[pinEIndex] = currentSegmentState[pinEIndex];
      return;
    }

    if (currentSegment == pinFIndex) {
      currentSegment = pinBIndex;
      segmentState[pinFIndex] = currentSegmentState[pinFIndex];
      return;
    }
  }

  if (direction == "up") {
    if (currentSegment == pinBIndex) {
      currentSegment = pinAIndex;
      segmentState[pinBIndex] = currentSegmentState[pinBIndex];
      return;
    }

    if (currentSegment == pinCIndex) {
      currentSegment = pinGIndex;
      segmentState[pinCIndex] = currentSegmentState[pinCIndex];
      return;
    }

    if (currentSegment == pinDIndex) {
      currentSegment = pinGIndex;
      segmentState[pinDIndex] = currentSegmentState[pinDIndex];
      return;
    }

    if (currentSegment == pinEIndex) {
      currentSegment = pinGIndex;
      segmentState[pinEIndex] = currentSegmentState[pinEIndex];
      return;
    }

    if (currentSegment == pinFIndex) {
      currentSegment = pinAIndex;
      segmentState[pinFIndex] = currentSegmentState[pinFIndex];
      return;
    }

    if (currentSegment == pinGIndex) {
      currentSegment = pinAIndex;
      segmentState[pinGIndex] = currentSegmentState[pinGIndex];
      return;
    }
  }

  if (direction == "down") {
    if (currentSegment == pinAIndex) {
      currentSegment = pinGIndex;
      segmentState[pinAIndex] = currentSegmentState[pinAIndex];
      return;
    }

    if (currentSegment == pinBIndex) {
      currentSegment = pinGIndex;
      segmentState[pinBIndex] = currentSegmentState[pinBIndex];
      return;
    }

    if (currentSegment == pinCIndex) {
      currentSegment = pinDIndex;
      segmentState[pinCIndex] = currentSegmentState[pinCIndex];
      return;
    }

    if (currentSegment == pinEIndex) {
      currentSegment = pinDIndex;
      segmentState[pinEIndex] = currentSegmentState[pinEIndex];
      return;
    }

    if (currentSegment == pinFIndex) {
      currentSegment = pinGIndex;
      segmentState[pinFIndex] = currentSegmentState[pinFIndex];
      return;
    }

    if (currentSegment == pinGIndex) {
      currentSegment = pinDIndex;
      segmentState[pinGIndex] = currentSegmentState[pinGIndex];
      return;
    }
  }
}

void checkState(byte currentState, byte currentSegment) {
  if (currentState == 1) {
    blinkLed(currentSegment);

    if (yValue < minThreshold && xValue < maxDiagonalThreshold && xValue > minDiagonalThreshold && joyMoved == false) {
      moveSegment("left");
      joyMoved = true;
    }

    if (yValue > maxThreshold && xValue < maxDiagonalThreshold && xValue > minDiagonalThreshold && joyMoved == false) {
      moveSegment("right");
      joyMoved = true;
    }

    if (xValue > maxThreshold && yValue < maxDiagonalThreshold && yValue > minDiagonalThreshold && joyMoved == false) {
      moveSegment("up");
      joyMoved = true;
    }

    if (xValue < minThreshold && yValue < maxDiagonalThreshold && yValue > minDiagonalThreshold && joyMoved == false) {
      moveSegment("down");
      joyMoved = true;
    }

    if (yValue > minThreshold && yValue < maxThreshold && xValue > minThreshold && xValue < maxThreshold) {
      joyMoved = false;
    }
  }

  else {
    segmentState[currentSegment] = currentSegmentState[currentSegment];
    displaySegments(segments);

    if (xValue > maxThreshold && yValue < maxDiagonalThreshold && yValue > minDiagonalThreshold) {
      currentSegmentState[currentSegment] = HIGH;
    }    
    else if (xValue < minThreshold && yValue < maxDiagonalThreshold && yValue > minDiagonalThreshold) {
      currentSegmentState[currentSegment] = LOW;
    }  
  }
}


void setup() {
  // initializing all the pins
  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }

  pinMode(pinSW, INPUT_PULLUP);

  if (commonAnode == true) {
    state = !state;
  }
}

void loop() {
  displaySegments(segments);
  checkState(currentState, currentSegment);

  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  byte reading = digitalRead(pinSW);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > debounceDelay) {
    if (reading != swState) {
      swState = reading;

      if (swState == LOW) {
        pressedTime = millis();
        isPressing = true;
      }
      if (swState == HIGH) {
        isPressing = false;

        if (millis() - pressedTime < longPressThreshold) {
          if (currentState == 1) {
            currentState = 2;
          }
          else {
            currentState = 1;
            segmentState[currentSegment] = currentSegmentState[currentSegment];
          }
        }
      }
    }
    if (isPressing == true && millis() - pressedTime > longPressThreshold && currentState == 1) {
      resetDisplay();
    }
  }

  lastReading = reading;
}