/*
  Electronic Dice v1.0

  Created by: Andres Garcia Alves <andres.garcia.alves@gmail.com>
  Microcontroller: Arduino Nano (ATMega328 @16Mhz)
  
  Version 1.0, 2022.02.23 - Initial release.
  Version 1.1, 2022.02.24 - Adding ON/OFF animations switch.
                            Fixed bug with random secuences (randomSeed).
  Version 1.2, 2022.02.27 - Re-arrange LEDs pin-out to simplify PCB.

  This source code is licensed under GPL v3.0  

  Pinout:
      *(A)        *(B)

      *(C)  *(D)  *(E)

      *(F)        *(G)

  D02 -> input with button interrupt
  D03 -> input with switch
  D04 -> output to bottom-right led (G)
  D05 -> output to middle-right led (E)
  D06 -> output to upper-right led (B)
  D07 -> output to middle-center led (D)
  D08 -> output to bottom-left led (F)
  D09 -> output to middle-left led (C)
  D10 -> output to upper-left led (A)
*/

#include <LowPower.h>


// function declarations
void buttonPressed();
void displayDiceNumber(long diceNumber);
void clearDice();
void animateDiceNumber(long diceNumber);

void displayAnimation(long animNumber);
void displayAnimation01();
void displayAnimation02();
void displayAnimation03();
void displayAnimation04();
void displayAnimation05();
void displayAnimation06();
void displayAnimation07();
void displayAnimation08();
void displayAnimation09();
void displayAnimation10();
void displayAnimation11();
void displayAnimation12();
void displayAnimation13();

// interrupt handler declarations
void btnInterruptHandler();
void switchInterruptHandler();

// const & variables
const int PIN_BTN = 2;
const int PIN_SWITCH = 3;

const int PIN_LED_A = 10;
const int PIN_LED_B = 6;
const int PIN_LED_C = 9;
const int PIN_LED_D = 7;
const int PIN_LED_E = 5;
const int PIN_LED_F = 8;
const int PIN_LED_G = 4;

const int DEBOUNCE_DELAY = 50;
const int DICE_DELAY = 5000;

const int ANIM_DELAY = 200;
const int ANIM_HALF_DELAY = 100;
const int ANIM_DOUBLE_DELAY = 400;
const int ANIM_ENDING_DELAY = 1000;


// interrupt handlers
void btnInterruptHandler() { }
 

// initial setup
void setup() {

  // CLKPR = 0x80; // (1000 0000) enable change in clock frequency
  // CLKPR = 0x01; // (0000 0001) use clock division factor 2 to reduce the frequency from 16 MHz to 8 MHz

  Serial.begin(9600);

  pinMode(PIN_BTN, INPUT_PULLUP);
  pinMode(PIN_SWITCH, INPUT_PULLUP);
  pinMode(PIN_LED_A, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  pinMode(PIN_LED_C, OUTPUT);
  pinMode(PIN_LED_D, OUTPUT);
  pinMode(PIN_LED_E, OUTPUT);
  pinMode(PIN_LED_F, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);

  // get some noise value from A0
  randomSeed(analogRead(A0));

  // enable external interrupts on pins 2 & 3
  attachInterrupt(digitalPinToInterrupt(PIN_BTN), btnInterruptHandler, FALLING);
}


// main loop
void loop() {

  // enter in power-down state
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_ON);

  // main button logic
  checkButtonPressed();
}


void checkButtonPressed() {

  // disable external interrupts on pin 2
  // detachInterrupt(digitalPinToInterrupt(PIN_BTN));

  delay(DEBOUNCE_DELAY); // de-bounce delay
 
  bool btnStatus = digitalRead(PIN_BTN);
  if (btnStatus != LOW) { return; }

  // throw the dice
  long diceNumber = random(1, 7); // 1 to 6
  long animNumber = random(1, 14); // 1 to 13

  // animation
  bool switchAnim = digitalRead(PIN_SWITCH);
  if (switchAnim == true) {
    displayAnimation(animNumber);
    delay(ANIM_ENDING_DELAY);
  }

  // display dice number & fade-out
  displayDiceNumber(diceNumber);
  delay(DICE_DELAY);
  animateDiceNumber(diceNumber);
}


void displayDiceNumber(long diceNumber) {

  clearDice();

  switch (diceNumber) {
    case 1:
      digitalWrite(PIN_LED_D, HIGH);
      break;
    case 2:
      digitalWrite(PIN_LED_A, HIGH);
      digitalWrite(PIN_LED_G, HIGH);
      break;
    case 3:
      digitalWrite(PIN_LED_A, HIGH);
      digitalWrite(PIN_LED_D, HIGH);
      digitalWrite(PIN_LED_G, HIGH);
      break;
    case 4:
      digitalWrite(PIN_LED_A, HIGH);
      digitalWrite(PIN_LED_B, HIGH);
      digitalWrite(PIN_LED_F, HIGH);
      digitalWrite(PIN_LED_G, HIGH);
      break;
    case 5:
      digitalWrite(PIN_LED_A, HIGH);
      digitalWrite(PIN_LED_B, HIGH);
      digitalWrite(PIN_LED_D, HIGH);
      digitalWrite(PIN_LED_F, HIGH);
      digitalWrite(PIN_LED_G, HIGH);
      break;
    case 6:
      digitalWrite(PIN_LED_A, HIGH);
      digitalWrite(PIN_LED_B, HIGH);
      digitalWrite(PIN_LED_C, HIGH);
      digitalWrite(PIN_LED_E, HIGH);
      digitalWrite(PIN_LED_F, HIGH);
      digitalWrite(PIN_LED_G, HIGH);
      break;
  }
}


void clearDice() {
  
  digitalWrite(PIN_LED_A, LOW);
  digitalWrite(PIN_LED_B, LOW);
  digitalWrite(PIN_LED_C, LOW);
  digitalWrite(PIN_LED_D, LOW);
  digitalWrite(PIN_LED_E, LOW);
  digitalWrite(PIN_LED_F, LOW);
  digitalWrite(PIN_LED_G, LOW);
}


void animateDiceNumber(long diceNumber) {
  
  for (int i = 0; i < 6; i++) {
    displayDiceNumber(diceNumber);
    delay(ANIM_DOUBLE_DELAY);
    clearDice();
    delay(ANIM_DELAY);
  }
}


// display an animation by number
void displayAnimation(long animNumber) {
  
  switch (animNumber) {
    case 1: displayAnimation01(); break;
    case 2: displayAnimation02(); break;
    case 3: displayAnimation03(); break;
    case 4: displayAnimation04(); break;
    case 5: displayAnimation05(); break;
    case 6: displayAnimation06(); break;
    case 7: displayAnimation07(); break;
    case 8: displayAnimation08(); break;
    case 9: displayAnimation09(); break;
    case 10: displayAnimation10(); break;
    case 11: displayAnimation11(); break;
    case 12: displayAnimation12(); break;
    case 13: displayAnimation13(); break;
  }
}


// anim #1: loop row-by-row from upper to bottom (18 ticks)
void displayAnimation01() {
  
  for (int i = 0; i < 6; i++) {
    digitalWrite(PIN_LED_A, HIGH);
    digitalWrite(PIN_LED_B, HIGH);
    delay(ANIM_DELAY);
    clearDice();
    digitalWrite(PIN_LED_C, HIGH);
    digitalWrite(PIN_LED_D, HIGH);
    digitalWrite(PIN_LED_E, HIGH);
    delay(ANIM_DELAY);
    clearDice();
    digitalWrite(PIN_LED_F, HIGH);
    digitalWrite(PIN_LED_G, HIGH);
    delay(ANIM_DELAY);
    clearDice();
  }
}


// anim #2: bounce row-by-row from upper to bottom (16 ticks)
void displayAnimation02() {
  
  for (int i = 0; i < 4; i++) {
    digitalWrite(PIN_LED_A, HIGH);
    digitalWrite(PIN_LED_B, HIGH);
    delay(ANIM_DELAY);
    clearDice();
    digitalWrite(PIN_LED_C, HIGH);
    digitalWrite(PIN_LED_D, HIGH);
    digitalWrite(PIN_LED_E, HIGH);
    delay(ANIM_DELAY);
    clearDice();
    digitalWrite(PIN_LED_F, HIGH);
    digitalWrite(PIN_LED_G, HIGH);
    delay(ANIM_DELAY);
    clearDice();
    digitalWrite(PIN_LED_C, HIGH);
    digitalWrite(PIN_LED_D, HIGH);
    digitalWrite(PIN_LED_E, HIGH);
    delay(ANIM_DELAY);
    clearDice();
  }
}


// anim #3: go one-by-one over leds perimeter (18 ticks)
void displayAnimation03() {
  
  for (int i = 0; i < 6; i++) {
    digitalWrite(PIN_LED_A, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_B, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_E, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_G, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_F, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_C, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();  }
}


// anim #4: go one-by-one over leds in zig-zag (17.5 ticks)
void displayAnimation04() {
  
  for (int i = 0; i < 5; i++) {
    digitalWrite(PIN_LED_A, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_B, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_C, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_D, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_E, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_F, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_G, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();  }
}


// anim #5: perimeter leds followed by the central one (16 ticks)
void displayAnimation05() {
  
  for (int i = 0; i < 8; i++) {
    digitalWrite(PIN_LED_A, HIGH);
    digitalWrite(PIN_LED_B, HIGH);
    digitalWrite(PIN_LED_C, HIGH);
    digitalWrite(PIN_LED_E, HIGH);
    digitalWrite(PIN_LED_F, HIGH);
    digitalWrite(PIN_LED_G, HIGH);
    delay(ANIM_DELAY);
    clearDice();
    digitalWrite(PIN_LED_D, HIGH);
    delay(ANIM_DELAY);
    clearDice();
  }
}


// anim #6: corner leds followed by the central one (16 ticks)
void displayAnimation06() {
  
  for (int i = 0; i < 8; i++) {
    digitalWrite(PIN_LED_A, HIGH);
    digitalWrite(PIN_LED_B, HIGH);
    digitalWrite(PIN_LED_F, HIGH);
    digitalWrite(PIN_LED_G, HIGH);
    delay(ANIM_DELAY);
    clearDice();
    digitalWrite(PIN_LED_D, HIGH);
    delay(ANIM_DELAY);
    clearDice();
  }
}


// anim #7: a dot going in vertical lines (18 ticks)
void displayAnimation07() {
  
  for (int i = 0; i < 6; i++) {
    digitalWrite(PIN_LED_A, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_C, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_F, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_B, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_E, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_G, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
  }
}


// anim #8: a dot going in horizontal lines (17.5 ticks)
void displayAnimation08() {
  
  for (int i = 0; i < 5; i++) {
    digitalWrite(PIN_LED_A, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_B, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_C, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_D, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_E, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_F, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_G, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
  }
}


// anim #9: a dot going in vertical lines followed by horizontal ones (19.5 ticks)
void displayAnimation09() {
  for (int i = 0; i < 3; i++) {
    // vertical move code
    digitalWrite(PIN_LED_A, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_C, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_F, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_B, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_E, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_G, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    // horizontal move code
    digitalWrite(PIN_LED_A, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_B, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_C, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_D, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_E, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_F, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
    digitalWrite(PIN_LED_G, HIGH);
    delay(ANIM_HALF_DELAY);
    clearDice();
  }
}


// anim #10: loop of horizontal and vertical lines (16 ticks)
void displayAnimation10() {
  
  for (int i = 0; i < 2; i++) {
    // horizontal upper line
    digitalWrite(PIN_LED_A, HIGH);
    digitalWrite(PIN_LED_B, HIGH);
    delay(ANIM_DELAY);
    clearDice();
    // vertical right line
    digitalWrite(PIN_LED_B, HIGH);
    digitalWrite(PIN_LED_E, HIGH);
    digitalWrite(PIN_LED_G, HIGH);
    delay(ANIM_DELAY);
    clearDice();
    // horizontal middle line
    digitalWrite(PIN_LED_C, HIGH);
    digitalWrite(PIN_LED_D, HIGH);
    digitalWrite(PIN_LED_E, HIGH);
    delay(ANIM_DELAY);
    clearDice();
    // vertical left line
    digitalWrite(PIN_LED_A, HIGH);
    digitalWrite(PIN_LED_C, HIGH);
    digitalWrite(PIN_LED_F, HIGH);
    delay(ANIM_DELAY);
    clearDice();
    // horizontal bottom line
    digitalWrite(PIN_LED_F, HIGH);
    digitalWrite(PIN_LED_G, HIGH);
    delay(ANIM_DELAY);
    clearDice();
    // vertical right line
    digitalWrite(PIN_LED_B, HIGH);
    digitalWrite(PIN_LED_E, HIGH);
    digitalWrite(PIN_LED_G, HIGH);
    delay(ANIM_DELAY);
    clearDice();
    // horizontal middle line
    digitalWrite(PIN_LED_C, HIGH);
    digitalWrite(PIN_LED_D, HIGH);
    digitalWrite(PIN_LED_E, HIGH);
    delay(ANIM_DELAY);
    clearDice();
    // vertical left line
    digitalWrite(PIN_LED_A, HIGH);
    digitalWrite(PIN_LED_C, HIGH);
    digitalWrite(PIN_LED_F, HIGH);
    delay(ANIM_DELAY);
    clearDice();
  }
}


// anim #11: random dots (18 ticks)
void displayAnimation11() {

  for (int i = 0; i < 36; i++) {
    long ledNumber = random(1, 8); // 1 to 7

    switch (ledNumber) {
      case 1: digitalWrite(PIN_LED_A, HIGH); break;
      case 2: digitalWrite(PIN_LED_B, HIGH); break;
      case 3: digitalWrite(PIN_LED_C, HIGH); break;
      case 4: digitalWrite(PIN_LED_D, HIGH); break;
      case 5: digitalWrite(PIN_LED_E, HIGH); break;
      case 6: digitalWrite(PIN_LED_F, HIGH); break;
      case 7: digitalWrite(PIN_LED_G, HIGH); break;
    }
    delay(ANIM_HALF_DELAY);
    clearDice();
  }
}


// anim #12: random dot fill (9 x 2 -> 18 ticks)
void displayAnimation12() {
  int idx;

  for (int i = 0; i < 2; i++) {
    bool empty[7] = {true, true, true, true, true, true, true};

    for (int j = 0; j < 7; j++) {
      do {
        idx = (int)random(0, 7); // 0 to 6
      } while (empty[idx] == false);
      empty[idx] = false;

      switch (idx) {
        case 0: digitalWrite(PIN_LED_A, HIGH); break;
        case 1: digitalWrite(PIN_LED_B, HIGH); break;
        case 2: digitalWrite(PIN_LED_C, HIGH); break;
        case 3: digitalWrite(PIN_LED_D, HIGH); break;
        case 4: digitalWrite(PIN_LED_E, HIGH); break;
        case 5: digitalWrite(PIN_LED_F, HIGH); break;
        case 6: digitalWrite(PIN_LED_G, HIGH); break;
      }
      delay(ANIM_DELAY);
    }

    delay(ANIM_DOUBLE_DELAY);
    clearDice();
  }
}


// anim #13: random dot fill & ramdom dot empty (9 + 9 -> 18 ticks)
void displayAnimation13() {
  int idx;
  bool empty[7] = {true, true, true, true, true, true, true};

  // random fill
  for (int i = 0; i < 7; i++) {
    do {
      idx = (int)random(0, 7); // 0 to 6
    } while (empty[idx] == false);
    empty[idx] = false;

    switch (idx) {
      case 0: digitalWrite(PIN_LED_A, HIGH); break;
      case 1: digitalWrite(PIN_LED_B, HIGH); break;
      case 2: digitalWrite(PIN_LED_C, HIGH); break;
      case 3: digitalWrite(PIN_LED_D, HIGH); break;
      case 4: digitalWrite(PIN_LED_E, HIGH); break;
      case 5: digitalWrite(PIN_LED_F, HIGH); break;
      case 6: digitalWrite(PIN_LED_G, HIGH); break;
    }
    delay(ANIM_DELAY);
  }

  delay(ANIM_DOUBLE_DELAY);

  // random empty
  for (int j = 0; j < 7; j++) {
    do {
      idx = (int)random(0, 7); // 0 to 6
    } while (empty[idx] == true);
    empty[idx] = true;

    switch (idx) {
      case 0: digitalWrite(PIN_LED_A, LOW); break;
      case 1: digitalWrite(PIN_LED_B, LOW); break;
      case 2: digitalWrite(PIN_LED_C, LOW); break;
      case 3: digitalWrite(PIN_LED_D, LOW); break;
      case 4: digitalWrite(PIN_LED_E, LOW); break;
      case 5: digitalWrite(PIN_LED_F, LOW); break;
      case 6: digitalWrite(PIN_LED_G, LOW); break;
    }
    delay(ANIM_DELAY);
  }

  delay(ANIM_DOUBLE_DELAY);
  clearDice();
}
