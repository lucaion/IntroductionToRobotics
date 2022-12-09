#include <LiquidCrystal.h>
#include <LedControl.h>

// matrix variables
const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;
byte matrixBrightness = 5;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No.DRIVER

// lcd variables
const byte RS = 9;
const byte enable = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

// joystick variables
byte swState = LOW;
int xValue = 0;
int yValue = 0;
const int pinSW = 2; 
const int pinX = A0; 
const int pinY = A1;

// variables for joystick moves
bool joyMoved = false;
const int minThreshold = 200;
const int maxThreshold = 800;
const int minDiagonalThreshold = 400;
const int maxDiagonalThreshold = 600;

// debounce variables
const int debounceDelay = 50;
unsigned long int lastDebounceTime = 0;
const int longPressThreshold = 2000;
volatile byte lastReading = LOW;
byte reading = LOW;
unsigned long pressedTime = 0;
bool isPressing = false;

const byte mainMenuSize = 5;

String mainMenu[mainMenuSize] = {
  "Start game",
  "Leaderboard",
  "Settings",
  "About",
  "How to play"
};

const byte settingsMenuSize = 6;
String settingsMenu[settingsMenuSize] = {
  "Enter name",
  "Difficulty",
  "LCD contrast",
  "LCD brightness",
  "Mat. brightness",
  "Sound"
};

const long delayVariable = 4000;


byte currentMenu = 0;
byte lastCursorState;
byte currentCursorState = 0;
bool playingGame = false;
long score = 0;

byte rightArrow[] = {
  B00000,
  B00100,
  B00110,
  B11111,
  B11111,
  B00110,
  B00100,
  B00000
};

byte matrix[matrixSize][matrixSize] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};

byte xPos = 0;
byte yPos = 0;
byte xLastPos = 0;
byte yLastPos = 0;
byte xFood = random(8);
byte yFood = random(8);
byte xLastFood = 0;
byte yLastFood = 0;
unsigned long previousMillis = 0;
const byte moveInterval = 100;
const long interval = 300;
unsigned long long lastMoved = 0;
bool matrixChanged = true;



void setup() {
  pinMode(pinSW, INPUT_PULLUP);
  lcd.begin(16, 2);
  lcd.createChar(1, rightArrow);

  lcd.begin(16, 2);
  lcd.print("--- GameName ---");
  lcd.setCursor(4, 1);
  lcd.print("WELCOME!");
  delay(delayVariable);
  lcd.clear();

  lc.shutdown(0, false);
  lc.setIntensity(0, matrixBrightness);
  lc.clearDisplay(0);
  matrix[xPos][yPos] = 1;
  matrix[xFood][yFood] = 1;
}

void loop() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  reading = digitalRead(pinSW);
  if (!playingGame) {
    handleMenu();
    menuUpDownMoves(mainMenuSize);
  }
  else {
    playGame();
    menuInGame();
  }
}

void handleMenu() {
  buttonPressed();
  switch (currentMenu) {
    case 0:
      displayMainMenu();
      break;
    case 1:
      displayLeaderboard();
      break;
    case 2:
      displaySettings();
      break;
    case 3:
      displayAbout();
      break;
    case 4:
      displayHowToPlay();
      break;
  }
}


void displayMainMenu() {
  buttonPressed();
  lcd.setCursor(4, 0);
  lcd.print("GameName");      
  lcd.setCursor(0, 1);
  lcd.write(1);
  lcd.setCursor(2, 1);
  lcd.print(mainMenu[currentCursorState]);
}

void displayLeaderboard(){
  buttonPressed();
  lcd.setCursor(0, 0);
  lcd.print("Leaderboard");
  lcd.setCursor(0, 1);
  lcd.print("page");
}

void displaySettings(){
  buttonPressed();
  lcd.setCursor(4, 0);
  lcd.print("Settings");      
  lcd.setCursor(0, 1);
  lcd.print("page");
}

void displayAbout(){
  buttonPressed();
  lcd.setCursor(0, 0);
  lcd.print("About");
  lcd.setCursor(0, 1);
  lcd.print("page");
}

void displayHowToPlay(){
  buttonPressed();
  lcd.setCursor(0, 0);
  lcd.print("Move Joystick");
  lcd.setCursor(0, 1);
  lcd.print("eat blinking led");
}


void menuUpDownMoves(byte menuSize) {
  if (currentMenu == 0) {
    if (xValue < minThreshold && yValue < maxDiagonalThreshold && yValue > minDiagonalThreshold && joyMoved == false) {
      if (currentCursorState == menuSize - 1) {
        return;        
      }
      else {
        currentCursorState++;
        lcd.clear();
        displayMainMenu();
      }
      joyMoved = true;
    }    
    if (xValue > maxThreshold && yValue < maxDiagonalThreshold && yValue > minDiagonalThreshold && joyMoved == false) {
      if (currentCursorState == 0) {
        return;
      }
      else {
        currentCursorState--; 
        lcd.clear();
        displayMainMenu();
      }
      joyMoved = true;
    }

    if (yValue > minThreshold && yValue < maxThreshold && xValue > minThreshold && xValue < maxThreshold) {
      joyMoved = false;
    }  
  } 
  
}


void playGame(){
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();

    if (!matrix[xFood][yFood]) {
      matrix[xFood][yFood] = 1;
      lc.setLed(0, xFood, yFood, 1);
    } else {
      matrix[xFood][yFood] = 0;
      lc.setLed(0, xFood, yFood, 0);
    }
  }

  if(millis() - lastMoved > moveInterval) {
    // game logic
    updatePositions();
    lastMoved = millis();
  }

  if(matrixChanged == true) {
    // matrix display logic
    updateMatrix();
    matrixChanged = false;
  }

  if (xPos == xFood && yPos == yFood) {
    score++;
    generateFood();
  }
}

void generateFood() {
  xFood = random(8);
  yFood = random(8);
  matrix[xFood][yFood] = 1;
  matrixChanged = true;
}

void updateMatrix() {
  for(int row = 0; row < matrixSize; row++) {
    for(int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
}


void updatePositions() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  xLastPos = xPos;
  yLastPos = yPos;
  
  if(xValue < minThreshold) {
    if(xPos < matrixSize - 1) {
      xPos++;
    }
    else {
      xPos = 0;
    }
  }

  if(xValue > maxThreshold) {
    if(xPos > 0) {
      xPos--;
    }
    else {
      xPos = matrixSize - 1;
    }
  }
  
  if(yValue > maxThreshold) {
    if(yPos < matrixSize - 1) {
      yPos++;
    }
    else {
      yPos = 0;
    }
  }
  
  if(yValue < minThreshold) {
    if(yPos > 0) {
      yPos--;
    }
    else {
      yPos = matrixSize - 1;
    }
  }
  
  if(xPos != xLastPos || yPos != yLastPos) {
    matrixChanged = true;
    matrix[xLastPos][yLastPos] = 0;
    matrix[xPos][yPos] = 1;
  }
}

void buttonPressed() {
  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > debounceDelay) {
    if (reading != swState) {
      swState = reading;

      if (swState == LOW) {
        if (currentMenu == 0 && currentCursorState == 0) {
          if (!playingGame) {
            playingGame = true;
          }
          else {
            playingGame = false;
          }
        }
        else {
          currentMenu = currentCursorState;
          lastCursorState = currentCursorState;
          currentCursorState = 0;
        }
        lcd.clear();
      }
    }
  }

  lastReading = reading;
}

void menuInGame(){
  lcd.setCursor(0, 0);
  lcd.print("--- GameName ---");
  lcd.setCursor(0, 1);
  lcd.print("Player");
  lcd.write(1);
  lcd.print("Score: ");
  lcd.print(score);
  buttonPressed();
}