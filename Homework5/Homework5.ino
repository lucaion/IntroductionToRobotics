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
const int maxThreshold = 750;
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


// menu size variables
const byte mainMenuSize = 5;
const byte settingsMenuSize = 7;
const byte aboutMenuSize = 4;
const byte howToPlayMenuSize = 0;

// lcd variables
const byte lcdWidth = 16;
const byte lcdHeight = 2;

unsigned long lastScrollTime;
int scrollCount;

// String mainMenu[mainMenuSize] = {
//   "Start game",
//   "Leaderboard",
//   "Settings",
//   "About",
//   "How to play"
// };


// String settingsMenu[settingsMenuSize] = {
//   "Enter name",
//   "Difficulty",
//   "LCD contrast",
//   "LCD brightness",
//   "Mat. brightness",
//   "Sound",
//   "Back"
// };

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

byte downArrow[] = {
  B00000,
  B01110,
  B01110,
  B01110,
  B11111,
  B01110,
  B00100,
  B00000
};

byte upArrow[] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B01110,
  B01110,
  B01110,
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

const int startGameMatrix[matrixSize] = {
  B00100000,
  B00110000,
  B00111000,
  B00111100,
  B00111000,
  B00110000,
  B00100000,
  B00000000
};

const int leaderboardMatrix2[matrixSize] = {
  B11000011,
  B01000101,
  B11100010,
  B00110111,
  B00001000,
  B00010000,
  B00001000,
  B00110000
};

const int leaderboardMatrix[matrixSize] = {
  B00000000,
  B00000000,
  B00011000,
  B01111000,
  B01111110,
  B01111110,
  B00000000,
  B00000000
};

const int settingsMatrix[matrixSize] = {
  B00000100,
  B00001000,
  B00001001,
  B00001110,
  B01110000,
  B10010000,
  B00010000,
  B00100000
};

const int howToPlayMatrix[matrixSize] = {
  B00000000,
  B00111000,
  B01000100,
  B00001000,
  B00010000,
  B00010000,
  B00000000,
  B00010000
};

const int aboutMatrix[matrixSize] = {
  B00000000,
  B00010000,
  B00000000,
  B00110000,
  B00010000,
  B00010000,
  B00011000,
  B00000000
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

const int buzzerPin = 13;




void setup() {
  pinMode(pinSW, INPUT_PULLUP);
  lcd.begin(lcdWidth, lcdHeight);
  lcd.createChar(1, rightArrow);
  lcd.createChar(2, downArrow);
  lcd.createChar(3, upArrow);

  lcd.begin(lcdWidth, lcdHeight);
  lcd.setCursor(3, 0);
  lcd.print("WELCOME TO");
  lcd.setCursor(0, 1);
  lcd.print("Snake Remastered");
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
      menuUpDownMoves(mainMenuSize);
      break;
    case 1:
      displayLeaderboard();
      break;
    case 2:
      displaySettings();
      menuUpDownMoves(settingsMenuSize);
      break;
    case 3:
      displayAbout();
      menuUpDownMoves(aboutMenuSize);
      break;
    case 4:
      displayHowToPlay();
      menuUpDownMoves(howToPlayMenuSize);
      break;
  }
}


void displayMainMenu() {
  if (currentMenu == 0){
    switch(currentCursorState) {
      // case 0:
      //   currentCursorState = 1;
      //   break;
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("Snake Remastered");
        lcd.setCursor(0, 1);
        lcd.write(1);
        lcd.setCursor(2, 1);
        lcd.print("Start Game");
        for (int row = 0; row < matrixSize; row++) {
          lc.setRow(0, row, startGameMatrix[row]);
        }
        lcd.setCursor(15, 1);
        lcd.write(2);
        break;
      case 1:
        lcd.setCursor(0, 0);
        lcd.write(1);
        lcd.setCursor(2, 0);
        lcd.print("Leaderboard");
        for (int row = 0; row < matrixSize; row++) {
          lc.setRow(0, row, leaderboardMatrix[row]);
        }
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(2, 1);
        lcd.print("Settings");
        lcd.setCursor(15, 1);
        lcd.write(2);
        break;
      case 2:
        lcd.setCursor(2, 0);
        lcd.print("Leaderboard");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(0, 1);
        lcd.write(1);
        lcd.setCursor(2, 1);
        lcd.print("Settings");
        for (int row = 0; row < matrixSize; row++) {
          lc.setRow(0, row, settingsMatrix[row]);
        }
        lcd.setCursor(15, 1);
        lcd.write(2);
        break;
      case 3:
        lcd.setCursor(0, 0);
        lcd.write(1);
        lcd.setCursor(2, 0);
        lcd.print("About");
        for (int row = 0; row < matrixSize; row++) {
          lc.setRow(0, row, aboutMatrix[row]);
        }
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(2, 1);
        lcd.print("How to play");
        lcd.setCursor(15, 1);
        lcd.write(2);
        break;
      case 4:
        lcd.setCursor(2, 0);
        lcd.print("About");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(0, 1);
        lcd.write(1);
        lcd.setCursor(2, 1);
        lcd.print("How to play");
        for (int row = 0; row < matrixSize; row++) {
          lc.setRow(0, row, howToPlayMatrix[row]);
        } 
        break;     
    }
  }
}

void displayLeaderboard(){
  buttonPressed();
  lcd.setCursor(0, 0);
  lcd.print("Leaderboard page");
  lcd.setCursor(0, 1);
  lcd.print("page");
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, leaderboardMatrix[row]);
  } 
}

void displaySettings(){
  if (currentMenu == 2){
    switch(currentCursorState) {
      case 0:
        lcd.setCursor(4, 0);
        lcd.print("Settings");
        lcd.setCursor(0, 1);
        lcd.write(1);
        lcd.setCursor(2, 1);
        lcd.print("Enter name");
        lcd.setCursor(15, 1);
        lcd.write(2);
        break;
      case 1:
        lcd.setCursor(0, 0);
        lcd.write(1);
        lcd.setCursor(2, 0);
        lcd.print("Difficulty");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(2, 1);
        lcd.print("LCD contrast");
        lcd.setCursor(15, 1);
        lcd.write(2);
        break;
      case 2:
        lcd.setCursor(2, 0);
        lcd.print("Difficulty");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(0, 1);
        lcd.write(1);        
        lcd.setCursor(2, 1);
        lcd.print("LCD contrast");
        lcd.setCursor(15, 1);
        lcd.write(2);
        break;
      case 3:
        lcd.setCursor(0, 0);
        lcd.write(1);
        lcd.setCursor(2, 0);
        lcd.print("LCDBrightness");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(2, 1);
        lcd.print("MatBrightness");
        lcd.setCursor(15, 1);
        lcd.write(2);
        break;
      case 4:
        lcd.setCursor(2, 0);
        lcd.print("LCDBrightness");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(0, 1);
        lcd.write(1);        
        lcd.setCursor(2, 1);
        lcd.print("MatBrightness"); 
        lcd.setCursor(15, 1);
        lcd.write(2);        
        break;     
      case 5:
        lcd.setCursor(0, 0);
        lcd.write(1); 
        lcd.setCursor(2, 0);
        lcd.print("Sound");
        lcd.setCursor(15, 0);
        lcd.write(3);   
        lcd.setCursor(2, 1);
        lcd.print("Back");
        lcd.setCursor(15, 1);
        lcd.write(2);
        break;      
      case 6:
        lcd.setCursor(2, 0);
        lcd.print("Sound");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(0, 1);
        lcd.write(1);
        lcd.setCursor(2, 1);
        lcd.print("Back"); 
        break;            
    }

    for (int row = 0; row < matrixSize; row++) {
      lc.setRow(0, row, settingsMatrix[row]);
    }
  }
}

void displayAbout(){
  if (currentMenu == 3) {
    switch (currentCursorState) {
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("-- About page --");
        lcd.setCursor(0, 1);
        lcd.write(1);
        //scrollText("Title: Snake Remastered");
        lcd.setCursor(2, 1);
        lcd.print("Title: Snake"); // Remastered");
        lcd.setCursor(15, 1);
        lcd.write(2);
        break;
      case 1: 
        lcd.setCursor(0, 0);
        lcd.write(1);
        lcd.setCursor(2, 0);
        lcd.print("By: Luca Ion");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(2, 1);
        lcd.print("Github: http");//s://github.com/lucaion");
        // lcd.setCursor(14, 1);
        // lcd.print("  ");
        lcd.setCursor(15, 1);
        lcd.write(2);
        break;
      case 2:
        lcd.setCursor(2, 0);
        lcd.print("By: Luca Ion");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(0, 1);
        lcd.write(1);
        lcd.setCursor(2, 1);
        lcd.print("Github: http");//s://github.com/lucaion");
        // lcd.setCursor(14, 1);
        // lcd.print("  ");
        lcd.setCursor(15, 1);
        lcd.write(2);
        break;
      case 3:
        lcd.setCursor(2, 0);
        lcd.print("Github: http");//s://github.com/lucaion");
        // lcd.setCursor(14, 0);
        // lcd.print("  ");
        lcd.setCursor(15, 0);
        lcd.write(3);
        lcd.setCursor(0, 1);
        lcd.write(1);
        lcd.setCursor(2, 1);
        lcd.print("Back");
        break;
    }

    for (int row = 0; row < matrixSize; row++) {
      lc.setRow(0, row, aboutMatrix[row]);
    } 
  }
}

void displayHowToPlay(){
  buttonPressed();
  lcd.setCursor(0, 0);
  lcd.print("Move Joystick");
  lcd.setCursor(0, 1);
  lcd.print("eat blinking led");
  for (int row = 0; row < matrixSize; row++) {
    lc.setRow(0, row, howToPlayMatrix[row]);
  } 
}


void menuUpDownMoves(byte menuSize) {
  if (xValue < minThreshold && yValue < maxDiagonalThreshold && yValue > minDiagonalThreshold && joyMoved == false) {
    if (currentCursorState == menuSize - 1) {
      tone(buzzerPin, 370, 30);   
    }
    else {
      currentCursorState++;
      tone(buzzerPin, 600, 30);
      lcd.clear();
      lc.clearDisplay(0);
      displayMainMenu();
      displaySettings();
      displayAbout();
    }
    joyMoved = true;
  }    
  if (xValue > maxThreshold && yValue < maxDiagonalThreshold && yValue > minDiagonalThreshold && joyMoved == false) {
    if (currentCursorState == 0) {
      tone(buzzerPin, 370, 30);
    }
    else {
      currentCursorState--; 
      tone(buzzerPin, 600, 30);
      lcd.clear();
      lc.clearDisplay(0);
      displayMainMenu();
      displaySettings();
      displayAbout();
    }
    joyMoved = true;
  }

  if (yValue > minThreshold && yValue < maxThreshold && xValue > minThreshold && xValue < maxThreshold) {
    joyMoved = false;
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
        tone(buzzerPin, 105, 30);
        switch (currentMenu) {
          case 0: //mainMenu
            switch (currentCursorState) {
              case 0:
                if (!playingGame) {
                  playingGame = true;
                }
                else {
                  playingGame = false;
                }
                break;
              case 1: case 2: case 3: case 4:
                currentMenu = currentCursorState;
                lastCursorState = currentCursorState;
                currentCursorState = 0;
                break;
            }
            break;
          case 2: //settingsMenu
            if (currentCursorState == settingsMenuSize - 1) {
              currentMenu = 0;
              currentCursorState = lastCursorState;
            }
            break;
          case 3: // aboutMenu
            if (currentCursorState == aboutMenuSize - 1) {
              currentMenu = 0;
              currentCursorState = lastCursorState;
            }
            break;
        }
        // if (currentMenu == 0 && currentCursorState == 0) {
        //   if (!playingGame) {
        //     playingGame = true;
        //   }
        //   else {
        //     playingGame = false;
        //   }
        // }
        // else {
        //   currentMenu = currentCursorState;
        //   lastCursorState = currentCursorState;
        //   currentCursorState = 0;
        // }
        lcd.clear();
        lc.clearDisplay(0);
      }
    }
  }

  lastReading = reading;
}

void menuInGame(){
  lcd.setCursor(0, 0);
  lcd.print("Snake Remastered");
  lcd.setCursor(0, 1);
  lcd.print("Player");
  lcd.write(1);
  lcd.print("Score: ");
  lcd.print(score);
  buttonPressed();
}

void scrollText(String text) {
  while (text != " ") {
    int stringStart = 0;
    int stringEnd = 0;
    int scrollCursor = lcdWidth;

    lcd.setCursor(2, 1);
    lcd.print(text.substring(stringStart, stringEnd));
    delay(1500);

    if (stringStart == 0 && scrollCursor > 0) {
      scrollCursor--;
      stringEnd ++;

    }
    else if (stringStart == stringEnd) {
      stringStart = stringEnd = 0;
      scrollCursor = lcdWidth;
    }
    else if(stringEnd == text.length() && scrollCursor == 0) {
      stringStart++;
    }
    else {
      stringStart++;
      stringEnd++;
    }
  }
}