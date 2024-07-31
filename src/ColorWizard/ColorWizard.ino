// define the duration of button presses (in milliseconds)
#define PRESS_MEDIUM 750
#define PRESS_LONG 1500

// define pins
const char redPins[4] = {5, 2, 11, 8};
const char greenPins[4] = {6, 3, 12, 9};
const char bluePins[4] = {7, 4, 13, 10};

const char buttonPins[4] = {A3, A1, A2, A0};
const char buzzerPin = A4;

// how to represent colors
typedef char Color;
#define RED 0
#define GREEN 1
#define BLUE 2

// structure to represent a step of the sequence
struct Step {
  char position;
  Color color;
};

// game sequence and current level (global)
#define GAME_LENGTH 64
Step sequence[GAME_LENGTH];
unsigned char level = 0;

void setup() {
  // setup pins
  for (int i = 0; i < 4; i++) {
    pinMode(redPins[i], OUTPUT);
    pinMode(greenPins[i], OUTPUT);
    pinMode(bluePins[i], OUTPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(buzzerPin, OUTPUT);

  // initializes randomness; note: A5 must be an unconnected pin to have randomness
  randomSeed(analogRead(A5));

  // level == 0
  nextLevel();
}

void loop() {
  delay(1000);
  playSequence();
  if (getUserInput()) {
    if (level == GAME_LENGTH) {
      winGame();
    } else {
      nextLevel();      
    }
  } else {
    gameOver();
  }
}

void nextLevel() {
  sequence[level].position = random(0, 4);
  sequence[level].color = (Color)random(0, 3);
  level++;
}

void playSequence() {
  for (int i = 0; i < level; i++) {
    lightLED(sequence[i].position, sequence[i].color);
    delay(1000);
    turnOffLED(sequence[i].position);
    delay(500);
  }
}

bool getUserInput() {
  int i, j;
  long pressDuration, pressStartTime = millis();
  Color userColor;
  bool change, inputReceived;
  
  for (i = 0; i < level; i++) {
    inputReceived = false;

    while (!inputReceived) {

      for (j = 0; j < 4; j++) {
        
        if (digitalRead(buttonPins[j]) == LOW) {
          pressStartTime = millis();
          userColor = RED;
          change = true;  // to setup color for the first time
          
          while (digitalRead(buttonPins[j]) == LOW) {

            pressDuration = millis() - pressStartTime;

            if (pressDuration >= PRESS_LONG) {
              if (userColor != BLUE) {
                userColor = BLUE;
                change = true;
              }
            }
            else if (pressDuration >= PRESS_MEDIUM) {
              if (userColor != GREEN) {
                userColor = GREEN;
                change = true;
              }              
            }

            if (change) {
              lightLED(j, userColor);  // show color in real time
              change = false;              
            }
            
          }

          turnOffLED(j);  // turn off LED when button is released

          // stop the game as soon as a LED is wrong (position and/or color)
          if ((j != sequence[i].position) || (userColor != sequence[i].color)) {
            return false;
          }
          
          inputReceived = true;
          break;          
        }

      }

    }
    delay(100);

  }
  return true;
}

void lightLED(int position, Color color) {
  switch (color) {
    case RED:
      digitalWrite(redPins[position], HIGH);
      digitalWrite(greenPins[position], LOW);
      digitalWrite(bluePins[position], LOW);
      break;
    case GREEN:
      digitalWrite(redPins[position], LOW);
      digitalWrite(greenPins[position], HIGH);
      digitalWrite(bluePins[position], LOW);
      break;
    case BLUE:
      digitalWrite(redPins[position], LOW);
      digitalWrite(greenPins[position], LOW);
      digitalWrite(bluePins[position], HIGH);
      break;
  }
  tone(buzzerPin, 1600, 50);
}

void turnOffLED(int position) {
  digitalWrite(redPins[position], LOW);
  digitalWrite(greenPins[position], LOW);
  digitalWrite(bluePins[position], LOW);
  noTone(buzzerPin);
}

void gameOver() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      lightLED(j, RED);
    }
    tone(buzzerPin, 600, 200);
    delay(220);
    tone(buzzerPin, 300, 300);
    delay(330);
    for (int j = 0; j < 4; j++) {
      turnOffLED(j);
    }
    delay(500);
  }

  // restart
  level = 0;
  nextLevel();
}

void winGame() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 4; j++) {
      lightLED(j, GREEN);
    }
    tone(buzzerPin, 800, 200);
    delay(220);
    tone(buzzerPin, 1600, 300);
    delay(330);
    for (int j = 0; j < 4; j++) {
      turnOffLED(j);
    }
    delay(500);
  }

  // restart
  level = 0;
  nextLevel();
}
