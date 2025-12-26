/*
  Electronic Dice v1.3

  Created by: Andres Garcia Alves <andres.garcia.alves@gmail.com>
  Microcontroller: Arduino Nano (ATMega328 @16Mhz)
  
  Version 1.0, 2022.02.23 - Initial release.
  Version 1.1, 2022.02.24 - Adding ON/OFF animations switch.
                            Fixed bug with random secuences (randomSeed).
  Version 1.2, 2022.02.27 - Re-arrange LEDs pin-out to simplify PCB.
  Version 1.3, 2022.03.09 - Animations improvement using ROM tables: lines count 622->342, compiled total size 6031->4867 bytes (20% reduction).
  Version 1.4, 2022.03.10 - Dice number improvement using masked bits: lines count 342->296, compiled total size 4867->4777 bytes (2% reduction).

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

const int ANIM_COUNT = 13;
const int ANIM_SPEED_IDX = 0;
const int ANIM_LOOPS_IDX = 1;
const int ANIM_LENGHT_IDX = 2;

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

  // de-bounce delay
  delay(DEBOUNCE_DELAY);
  bool btnStatus = digitalRead(PIN_BTN);
  if (btnStatus != LOW) { return; }

  // throw the dice
  byte diceNumber = (byte)random(1, 7); // range 1-6
  byte animNumber = (byte)random(1, ANIM_COUNT+1); // range 1-ANIM_COUNT

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


void displayDiceNumber(byte diceNumber) {

  clearDice();

  // dice numbers: 0 (invalid), 1, 2, 3, 4, 5, 6
  byte numbersData[7] = { 0b00000000, 0b00001000, 0b01000001, 0b01001001, 0b01100011, 0b01101011, 0b01110111 };

  // set individuals leds ON/OFF by mask (format: 0XXXXXXX)
  displayLedsByMaskedBits(numbersData[diceNumber], HIGH);
}

void animateDiceNumber(byte diceNumber) {
  
  for (int i = 0; i < 6; i++) {
    displayDiceNumber(diceNumber);
    delay(ANIM_DOUBLE_DELAY);
    clearDice();
    delay(ANIM_DELAY);
  }
}


// display an animation by number
void displayAnimation(byte animNumber) {
  
  switch (animNumber) {
    case 1 ... 10: displayStaticAnimation(animNumber); break;
    case 11: displayDynamicAnimation1(); break;
    case 12: displayDynamicAnimation2(); break;
    case 13: displayDynamicAnimation3(); break;
  }
}

// anims #01-10: static animations
void displayStaticAnimation(byte animNumber) {

  // row format: [speed], [loops], [length], [bitmask step #1], [bitmask step #2], [bitmask step #n], [0]
  byte animationsData[10][16] = {
    { 1,  8,  2,  99,   8,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // #01: corner leds followed by the central one (16 ticks)
    { 1,  8,  2, 119,   8,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // #02: perimeter leds followed by the central one (16 ticks)
    { 1,  6,  3,   3,  28,  96,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // #03: loop row-by-row from upper to bottom (18 ticks)
    { 1,  4,  4,   3,  28,  96,  28,   0,   0,   0,   0,   0,   0,   0,   0,   0 }, // #04: bounce row-by-row from upper to bottom (16 ticks)
    { 0,  6,  6,   1,   2,  16,  64,  32,   4,   0,   0,   0,   0,   0,   0,   0 }, // #05: go one-by-one over leds perimeter (18 ticks)
    { 0,  5,  7,   1,   2,   4,   8,  16,  32,  64,   0,   0,   0,   0,   0,   0 }, // #06: a dot going in horizontal lines (17.5 ticks)
    { 0,  6,  6,   1,   4,  32,   2,  16,  64,   0,   0,   0,   0,   0,   0,   0 }, // #07: a dot going in vertical lines (18 ticks)
    { 0,  6,  6,   1,  16,  32,  64,   4,   2,   0,   0,   0,   0,   0,   0,   0 }, // #08: a dot going in zig-zag (18 ticks)
    { 1,  2,  8,   3,  82,  28,  37,  96,  82,  28,  37,   0,   0,   0,   0,   0 }, // #09: loop of horizontal and vertical lines (16 ticks)
    { 0,  3, 13,   1,   4,  32,   2,  16,  64,   1,   2,   4,   8,  16,  32,  64 }  // #10: a dot going in vertical lines followed by horizontal ones (19.5 ticks)
  };
  
  byte animStepData;
  byte animSpeed = animationsData[animNumber -1][ANIM_SPEED_IDX]; // idx 00
  byte animLoops = animationsData[animNumber -1][ANIM_LOOPS_IDX]; // idx 01
  byte animLenght = animationsData[animNumber -1][ANIM_LENGHT_IDX]; // idx 02

  for (int i = 0; i < animLoops; i++) {
    for (int j = 3; j < animLenght +3; j++) {

      // set individuals leds ON/OFF by mask
      animStepData = animationsData[animNumber -1][j];
      displayLedsByMaskedBits(animStepData, HIGH);
  
      delay(animSpeed == 0 ? ANIM_HALF_DELAY : ANIM_DELAY);
      clearDice();    
    }
  }
}

// anim #11: random dots (18 ticks)
void displayDynamicAnimation1() {

  for (int i = 0; i < 36; i++) {
    
    byte ledNumber = (byte)random(1, 8); // range 1-7
    displayLedByNumber(ledNumber, HIGH);
    
    delay(ANIM_HALF_DELAY);
    clearDice();
  }
}

// anim #12: random dots fill (9 x 2 -> 18 ticks)
void displayDynamicAnimation2() {
  byte idx;

  for (int i = 0; i < 2; i++) {
    bool empty[7] = {true, true, true, true, true, true, true};

    for (int j = 0; j < 7; j++) {
      do {
        idx = (byte)random(0, 7); // range 0-6
      } while (empty[idx] == false);
      empty[idx] = false;

      displayLedByNumber(idx+1, HIGH);
      delay(ANIM_DELAY);
    }

    delay(ANIM_DOUBLE_DELAY);
    clearDice();
  }
}

// anim #13: random dot fill & random dot empty (9 + 9 -> 18 ticks)
void displayDynamicAnimation3() {
  byte idx;
  bool empty[7] = {true, true, true, true, true, true, true};

  // random fill
  for (int i = 0; i < 7; i++) {
    do {
      idx = (byte)random(0, 7); // range 0-6
    } while (empty[idx] == false);
    empty[idx] = false;

    displayLedByNumber(idx+1, HIGH);
    delay(ANIM_DELAY);
  }

  delay(ANIM_DOUBLE_DELAY);

  // random empty
  for (int j = 0; j < 7; j++) {
    do {
      idx = (byte)random(0, 7); // range 0-6
    } while (empty[idx] == true);
    empty[idx] = true;

    displayLedByNumber(idx+1, LOW);
    delay(ANIM_DELAY);
  }

  delay(ANIM_DOUBLE_DELAY);
  clearDice();
}


// set all leds OFF
void clearDice() {

  // set individuals leds ON/OFF by mask (format: 0XXXXXXX)
  displayLedsByMaskedBits(0b11111111, LOW);
}

// set an individual led ON/OFF by number
void displayLedByNumber(byte ledNumber, boolean ledStatus) {
  
  switch (ledNumber) {
    case 1: digitalWrite(PIN_LED_A, ledStatus); break;
    case 2: digitalWrite(PIN_LED_B, ledStatus); break;
    case 3: digitalWrite(PIN_LED_C, ledStatus); break;
    case 4: digitalWrite(PIN_LED_D, ledStatus); break;
    case 5: digitalWrite(PIN_LED_E, ledStatus); break;
    case 6: digitalWrite(PIN_LED_F, ledStatus); break;
    case 7: digitalWrite(PIN_LED_G, ledStatus); break;
  }
}

// set individuals leds ON/OFF by mask (format: 0XXXXXXX)
void displayLedsByMaskedBits(byte maskedBits, boolean ledStatus) {
  
    if (maskedBits & 1) { digitalWrite(PIN_LED_A, ledStatus); }
    if (maskedBits & 2) { digitalWrite(PIN_LED_B, ledStatus); }
    if (maskedBits & 4) { digitalWrite(PIN_LED_C, ledStatus); }
    if (maskedBits & 8) { digitalWrite(PIN_LED_D, ledStatus); }
    if (maskedBits & 16) { digitalWrite(PIN_LED_E, ledStatus); }
    if (maskedBits & 32) { digitalWrite(PIN_LED_F, ledStatus); }
    if (maskedBits & 64) { digitalWrite(PIN_LED_G, ledStatus); }
}
