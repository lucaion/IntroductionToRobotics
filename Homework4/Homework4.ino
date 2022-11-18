// declaring all the joystick pins
const int pinSW = 2; 
const int pinX = A0; 
const int pinY = A1;

const int latchPin = 11; // STCP to 12 on Shift Register
const int clockPin = 10; // SHCP to 11 on Shift Register
const int dataPin = 12; // DS to 14 on Shift Register

const int segD1 = 7;
const int segD2 = 6;
const int segD3 = 5;
const int segD4 = 4;

int displayDigits[] = {
  segD1, segD2, segD3, segD4
};
const int displayCount = 4;

const int encodingsNumber = 16;

int byteEncodings[encodingsNumber] = {
//A B C D E F G DP 
  B11111100, // 0 
  B01100000, // 1
  B11011010, // 2
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5
  B10111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11110110, // 9
  B11101110, // A
  B00111110, // b
  B10011100, // C
  B01111010, // d
  B10011110, // E
  B10001110  // F
};

int currentNumber[displayCount] = {
  byteEncodings[0], byteEncodings[0], byteEncodings[0], byteEncodings[0]
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

byte currentDisplay = 3;
unsigned long lastBlink = 0;
const int blinkDelay = 300; 
byte currentState = 2;

const int debounceDelay = 50;
unsigned long int lastDebounceTime = 0;
const int longPressThreshold = 2000;
volatile byte lastReading = LOW;
unsigned long pressedTime = 0;
bool isPressing = false;


void blinkDP() {
  if (millis() - lastBlink >= blinkDelay) {
    lastBlink = millis();

    if (currentNumber[currentDisplay] % 2 == 0) {
      currentNumber[currentDisplay]++;
    } 
    else {
      currentNumber[currentDisplay]--;
    }
  } 
}

void turnDPOff() {
  if (currentNumber[currentDisplay] % 2 != 0) {
    currentNumber[currentDisplay]--;
  }
}

void turnDPOn() {
  if (currentNumber[currentDisplay] % 2 == 0) {
    currentNumber[currentDisplay]++;
  }
}

void activateDisplay(int digit) {
  for(int i = 0; i < displayCount; i++) {
    digitalWrite(displayDigits[i], HIGH);
  }

  digitalWrite(displayDigits[digit], LOW);
}

void resetDisplay() {
  for (int i = 0; i < displayCount; i++) {
    currentNumber[i] = byteEncodings[0];
  }
  currentDisplay = 3;
  currentState = 1;
}

void writeReg(int encoding) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, encoding);
  digitalWrite(latchPin, HIGH);
}

void writeNumber(int currentNumber[]) {
  int currentIndex = displayCount - 1;
  int displayIndex = 0;
  int lastCharacter = 0;
  
  while (currentIndex >= 0) {
    writeReg(currentNumber[currentIndex]);
    activateDisplay(displayIndex);
    displayIndex++;
    currentIndex--;
    delay(5);
  }
}

void changeNumber(int lastValue, char operation) {
  if (operation == '+') {
    if (lastValue != 15) {
      currentNumber[currentDisplay] = byteEncodings[lastValue + 1];
    }
    else {
      currentNumber[currentDisplay] = byteEncodings[0];
    }   
  }
  else if (operation == '-') {
    if (lastValue != 0) {
      currentNumber[currentDisplay] = byteEncodings[lastValue - 1];
    }
    else {
      currentNumber[currentDisplay] = byteEncodings[15];
    }    
  }
}

int searchNumber(int number) {
  for (int i = 0; i < encodingsNumber; i++) {
    if (byteEncodings[i] == number) {
      return i;
    }
  }
  return -1;
}

void moveNumber() {
  int lastValue = searchNumber(currentNumber[currentDisplay] - 1);

  if (xValue > maxThreshold && yValue < maxDiagonalThreshold && yValue > minDiagonalThreshold && joyMoved == false) {
    changeNumber(lastValue, '+');
    turnDPOn();
    joyMoved = true;
  }

  if (xValue < minThreshold && yValue < maxDiagonalThreshold && yValue > minDiagonalThreshold && joyMoved == false) {
    changeNumber(lastValue, '-');
    turnDPOn();
    joyMoved = true;
  }

  if (yValue > minThreshold && yValue < maxThreshold && xValue > minThreshold && xValue < maxThreshold) {
    joyMoved = false;
  }
}

void moveDP() {
  if (yValue < minThreshold && xValue < maxDiagonalThreshold && xValue > minDiagonalThreshold && joyMoved == false) {
    if(currentDisplay > 0) {
      turnDPOff();
      currentDisplay--;
    }
    joyMoved = true;
  }

  if (yValue > maxThreshold && xValue < maxDiagonalThreshold && xValue > minDiagonalThreshold && joyMoved == false) {
    if (currentDisplay < 3) {
      turnDPOff();
      currentDisplay++;
    }
    joyMoved = true;
  }

  if (yValue > minThreshold && yValue < maxThreshold && xValue > minThreshold && xValue < maxThreshold) {
    joyMoved = false;
  }
}

void checkState() {
  if (currentState == 1) {
    blinkDP();
    moveDP();
  }
  else {
    moveNumber();
  }
}

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(pinSW, INPUT_PULLUP);

  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }  
}

void loop() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  byte reading = digitalRead(pinSW);

  checkState();
  writeNumber(currentNumber);


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
            turnDPOn();
          }
          else {
            currentState = 1;
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