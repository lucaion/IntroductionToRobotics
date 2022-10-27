const int redPotPin = A0;
const int greenPotPin = A5;
const int bluePotPin = A2;

const int redLedPin = 11;
const int greenLedPin = 9;
const int blueLedPin = 10;

const int minAnalogValue = 0;
const int maxAnalogReadValue = 1023;
const int maxAnalogWriteValue = 255;

int redPotValue = 0;
int greenPotValue = 0;
int bluePotValue = 0;

int redLedValue = 0;
int greenLedValue = 0;
int blueLedValue = 0;


void setup() {
  pinMode(redPotPin, INPUT);
  pinMode(greenPotPin, INPUT);
  pinMode(bluePotPin, INPUT);

  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
}


void loop() {
  redPotValue = analogRead(redPotPin);
  greenPotValue = analogRead(greenPotPin);
  bluePotValue = analogRead(bluePotPin);

  redLedValue = map(redPotValue, minAnalogValue, maxAnalogReadValue, minAnalogValue, maxAnalogWriteValue);
  greenLedValue = map(greenPotValue, minAnalogValue, maxAnalogReadValue, minAnalogValue, maxAnalogWriteValue);
  blueLedValue = map(bluePotValue, minAnalogValue, maxAnalogReadValue, minAnalogValue, maxAnalogWriteValue);

  analogWrite(redLedPin, redLedValue);
  analogWrite(greenLedPin, greenLedValue);
  analogWrite(blueLedPin, blueLedValue);
}