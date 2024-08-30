#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int logicPins[4][3] = {
  {A0, A1, A2},
  {2, 3, 4},
  {5, 6, 7},
  {8, 9, 10}
};

const int notPins[6][2] = {
  {A0, A1},
  {A2, 2},
  {3, 4},
  {5, 7},
  {10, 6},
  {9, 8}
};

#define BUTTON1 11
#define BUTTON2 12

int gateMode = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);

  setupPins();
  displayMode();
}

void loop() {
  if (digitalRead(BUTTON1) == LOW) {
    delay(200);
    gateMode = (gateMode + 1) % 4;
    setupPins();
    displayMode();
  }

  if (digitalRead(BUTTON2) == LOW) {
    delay(200);
    runGateTest();
  }

  delay(100);
}

void setupPins() {
  for (int i = 0; i < 4; i++) {
    pinMode(logicPins[i][0], INPUT);
    pinMode(logicPins[i][1], INPUT);
    pinMode(logicPins[i][2], INPUT);
  }

  for (int i = 0; i < 6; i++) {
    pinMode(notPins[i][0], INPUT);
    pinMode(notPins[i][1], INPUT);
  }

  if (gateMode == 3) {
    for (int i = 0; i < 6; i++) {
      pinMode(notPins[i][0], OUTPUT);
      pinMode(notPins[i][1], INPUT);
    }
  } else {
    for (int i = 0; i < 4; i++) {
      pinMode(logicPins[i][0], OUTPUT);
      pinMode(logicPins[i][1], OUTPUT);
      pinMode(logicPins[i][2], INPUT);
    }
  }
}

void displayMode() {
  lcd.clear();
  lcd.setCursor(0, 0);
  const char* modes[] = {"Mode: OR", "Mode: AND", "Mode: NAND", "Mode: NOT"};
  lcd.print(modes[gateMode]);
}

void runGateTest() {
  lcd.clear();

  if (gateMode == 3) {
    for (int i = 0; i < 6; i++) {
      bool result = NOTCHECK(notPins[i][0], notPins[i][1]);
      lcd.setCursor((i % 3) * 5, i / 3);
      lcd.print("N");
      lcd.print(i + 1);
      lcd.print(result ? "P" : "F");
    }
  } else {
    for (int i = 0; i < 4; i++) {
      bool result = gateCheck(logicPins[i][0], logicPins[i][1], logicPins[i][2]);
      lcd.setCursor((i % 2) * 8, i / 2);
      lcd.print("G");
      lcd.print(i + 1);
      lcd.print(result ? " PASS" : " FAIL");
    }
  }

  delay(500);
}

bool gateCheck(int a, int b, int y) {
  switch (gateMode) {
    case 0: return ORCHECK(a, b, y);
    case 1: return ANDCHECK(a, b, y);
    case 2: return NANDCHECK(a, b, y);
  }
  return false;
}

bool ORCHECK(int a, int b, int y) {
  return (digitalWrite(a, LOW), digitalWrite(b, LOW), !digitalRead(y)) &&
         (digitalWrite(a, LOW), digitalWrite(b, HIGH), digitalRead(y)) &&
         (digitalWrite(a, HIGH), digitalWrite(b, LOW), digitalRead(y)) &&
         (digitalWrite(a, HIGH), digitalWrite(b, HIGH), digitalRead(y));
}

bool ANDCHECK(int a, int b, int y) {
  return (digitalWrite(a, LOW), digitalWrite(b, LOW), !digitalRead(y)) &&
         (digitalWrite(a, LOW), digitalWrite(b, HIGH), !digitalRead(y)) &&
         (digitalWrite(a, HIGH), digitalWrite(b, LOW), !digitalRead(y)) &&
         (digitalWrite(a, HIGH), digitalWrite(b, HIGH), digitalRead(y));
}

bool NANDCHECK(int a, int b, int y) {
  return (digitalWrite(a, LOW), digitalWrite(b, LOW), digitalRead(y)) &&
         (digitalWrite(a, LOW), digitalWrite(b, HIGH), digitalRead(y)) &&
         (digitalWrite(a, HIGH), digitalWrite(b, LOW), digitalRead(y)) &&
         (digitalWrite(a, HIGH), digitalWrite(b, HIGH), !digitalRead(y));
}

bool NOTCHECK(int a, int y) {
  digitalWrite(a, LOW);
  delay(10);
  bool result = digitalRead(y);
  digitalWrite(a, HIGH);
  delay(10);
  result &= !digitalRead(y);
  return result;
}
