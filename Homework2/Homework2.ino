const int buttonPin = 3;

const int greenLightPeoplePin = 4;
const int redLightPeoplePin = 5;

const int greenLightCarsPin = 6;
const int yellowLightCarsPin = 7;
const int redLightCarsPin = 8;

const int buzzerPin = 9;

// initial state
byte buttonState = LOW;
byte lastButtonState = LOW;

byte greenLightPeopleState = LOW;
byte redLightPeopleState = HIGH;

byte greenLightCarsState = HIGH;
byte yellowLightCarsState = LOW;
byte redLightCarsState = LOW;

// buzzer tones
const int buzzerTone = 150;
const int buzzerBlinkingTone = 400;

// debounce variables
unsigned int lastDebounceTime = 0;
unsigned int debounceDelay = 50;

unsigned long stateChangeTimer = 0;
int currentState = 1;

// delay of states
const int buttonPressDelay = 8000;
const int state2Duration = 3000;
const int state3Duration = 8000;
const int backToState1Delay = 12000;

unsigned long buzzerTimer = 0;
const int buzzerDelay = 1000;
const int blinkDelay = 350;


void lightsPrinting() {
  digitalWrite(greenLightPeoplePin, greenLightPeopleState);
  digitalWrite(redLightPeoplePin, redLightPeopleState);

  digitalWrite(greenLightCarsPin, greenLightCarsState);
  digitalWrite(yellowLightCarsPin, yellowLightCarsState);
  digitalWrite(redLightCarsPin, redLightCarsState);
}


// state 1: green light for cars, red light for people
void state1() {
  greenLightPeopleState = LOW;
  redLightPeopleState = HIGH;

  greenLightCarsState = HIGH;
  yellowLightCarsState = LOW;
  redLightCarsState = LOW;

  stateChangeTimer = 0;
  currentState = 1;
  buzzerTimer = 0;
}


// state2: yellow light for cars, red light for people
void state2() {
  greenLightCarsState = LOW;
  yellowLightCarsState = HIGH;

  stateChangeTimer = millis();
  currentState = 3;
}


// state3: red light for cars, green light for people and buzzer beeping
void state3() {
  greenLightPeopleState = HIGH;
  redLightPeopleState = LOW;

  yellowLightCarsState = LOW;
  redLightCarsState = HIGH;

  stateChangeTimer = millis();
  currentState = 4;
}


// state4: red light for cars, blinking green light for people and buzzer beeping faster
void state4() {
  if(millis() - buzzerTimer > blinkDelay) {
    tone(buzzerPin, buzzerBlinkingTone, blinkDelay);
    greenLightPeopleState = !greenLightPeopleState;
    buzzerTimer = millis();
  }
}


void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  
  pinMode(greenLightPeoplePin, OUTPUT);
  pinMode(redLightPeoplePin, OUTPUT);

  pinMode(greenLightCarsPin, OUTPUT);
  pinMode(yellowLightCarsPin, OUTPUT);
  pinMode(redLightCarsPin, OUTPUT);
}


void loop() {
  lightsPrinting();

  byte reading = digitalRead(buttonPin);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW && currentState == 1) {
        stateChangeTimer = millis();
        currentState = 2;
      }
    }
  }

  if (currentState == 2 && millis() - stateChangeTimer >= buttonPressDelay) {
    state2();
  }

  if (currentState == 3 && millis() - stateChangeTimer >= state2Duration) {
    state3();
  }

  if (currentState == 4 && millis() - stateChangeTimer < state3Duration) {
    if (millis() - buzzerTimer > buzzerDelay) {
      tone(buzzerPin, buzzerTone, 500);
      buzzerTimer = millis();
    }
  }

  if (currentState == 4 && millis() - stateChangeTimer >= state3Duration) {
    state4();
  }

  if (currentState == 4 && millis() - stateChangeTimer >= backToState1Delay) {
    state1();
  }

  lastButtonState = reading;
}