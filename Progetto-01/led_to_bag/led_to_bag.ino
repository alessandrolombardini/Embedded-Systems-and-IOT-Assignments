#include "MiniTimerOne.h" /* Library that manage the Timer1 of an ATmega328P processor on a Arduino Uno wrote by Matteo Castellucci and Giorgia Rondinini*/

#define DEFAULT_INTERRUPT_PERIOD 4000000 /* Base period of first round in microseconds */
#define DEBOUNCE_DELAY 150  /* Time to wait to avoid debounce problems */
#define L3_PIN 13
#define L2_PIN 12
#define L1_PIN 8
#define BAG_PIN 6
#define LOSE_PIN 4
#define POTENTIOMETER_PIN A0
#define START_BTN 3
#define MOVE_BTN 2
#define LED_BAG_INDEX 3
#define LED_LOSE_INDEX 4

// Functions declaration
void startGame(); /* Start the game */
void moveLed(); /* Move the switched on led to the next position during the blinking led phase */
void moveDown(); /* Move the switched on led to the next position during the game phase */
void interruptLose(); /* If the time of the round is spent, this method is called by interrupt to end the game (TimerOne) */
void readDifficulty(); /* Read the difficulty chosen by the user */
void refreshLed(); /* Update led status during the game phase */

// Global variables declaration
int leds[5] = {L3_PIN, L2_PIN, L1_PIN, BAG_PIN, LOSE_PIN}; /* All led pins */
int ledIndexOfBlinking = 0; /* Index of vector 'leds' of which led is switched on during the blinking phase */
int direction = 1; /* Direction of led movement during the blinking phase */
int difficulty = 0; /* Difficulty of the game: from 1 (EASY) to 8 (HARD) */
int fadeAmount = 5; /* Value to increas or decreas to manage duty cicle of white led */
int currIntensity = 0; /* Actual intensity of white led (PWM) */
int roundNumber = 0; /* Number of actual round in the game phase */
long prevTimeMoveDown = 0; /* Usefull to manage the bouncing of the move button */
long prevTimeStart = 0; /* Usefull to manage the bouncing of the game start button */
long actualPeriod = 0; /* Actual interrupt period - duration of this round */
bool inRound = false; /* TRUE: First led has alredy been switched on | FALSE: First led has to be switched on */
bool moveOn = false; /* TRUE: Fade of white led completed, play next round
                        FALSE: Fade of white led hasn't been completed yet, don't start next round */
volatile bool hasLose = false; /* TRUE: Player has lost | FALSE: Player hasn't lost yet */
volatile int currentLedIndex = 0; /* Index of vector 'leds' of which led is switched on during the game phase */
volatile bool inGame = false; /* TRUE: Game is on | FALSE: Game has to be started */

void setup() {
  pinMode(L1_PIN, OUTPUT);
  pinMode(L2_PIN, OUTPUT);
  pinMode(L3_PIN, OUTPUT);
  pinMode(BAG_PIN, OUTPUT);
  pinMode(LOSE_PIN, OUTPUT);
  pinMode(START_BTN, INPUT);
  pinMode(MOVE_BTN, INPUT);
  Serial.begin(9600);
  Serial.println("Welcome to Led to Bag. Press key TS to start");
  attachInterrupt(digitalPinToInterrupt(START_BTN), startGame, RISING);
  /* Initialize timer interrupt in order to manage the duration of rounds */
  MiniTimer1.init();
  MiniTimer1.attachInterrupt(interruptLose);
}

void loop() {
  /* Catch lose event */
  noInterrupts();
  bool hasLoseCopy = hasLose;
  interrupts();
  if (hasLoseCopy == true) { 
    detachInterrupt(digitalPinToInterrupt(MOVE_BTN));
    digitalWrite(L3_PIN, LOW);
    digitalWrite(L2_PIN, LOW);
    digitalWrite(L1_PIN, LOW);
    digitalWrite(BAG_PIN, LOW);
    digitalWrite(LOSE_PIN, HIGH);
    Serial.print("Game Over - Score: ");
    Serial.println(roundNumber);
    delay(2000); /* Red led is on for two seconds */
    digitalWrite(LOSE_PIN, LOW);
    roundNumber = 0;
    currentLedIndex = 0;
    currIntensity = 0;
    fadeAmount = 5;
    hasLose = false;
    moveOn = false;
    inRound = false;
    inGame = false;
    EIFR = _BV(INTF1); /* This call is used to free the interrupt buffer into ATmega328P pin 3.
                          Is usefull to be sure that the buffer doesn't remember possible interrupt on pin 3
                          occurred during a section where interrupts were disabled. This becouse ATmega328P
                          remember at most one interrupt call occurred during a no interrupt phase and, once
                          it exits by that section, it checks any calls made.*/
    attachInterrupt(digitalPinToInterrupt(START_BTN), startGame, RISING);
  }
  /* If the player isn't in game the three green leds blink */
  noInterrupts();
  bool inGameCopy = inGame;
  interrupts();
  if (!inGameCopy) {
    readDifficulty();
    moveLed();
    delay(100);
  } else { /* If the player is in game: */
    if (!inRound) { /* Need to switch on the first led once a round */
      detachInterrupt(digitalPinToInterrupt(START_BTN));
      digitalWrite(L3_PIN, LOW);
      digitalWrite(L2_PIN, LOW);
      digitalWrite(L1_PIN, LOW);
      digitalWrite(BAG_PIN, LOW);
      digitalWrite(LOSE_PIN, LOW);
      /* Switch on the first led */
      randomSeed(analogRead(5));
      currentLedIndex = random(3);
      digitalWrite(leds[currentLedIndex], HIGH);
      inRound = true;
      /* Set the duration of the round */
      calculatePeriod();
      MiniTimer1.setPeriod(actualPeriod);
      MiniTimer1.reset(); /* setPeriod() and start() don't set to zero the counter of TimerOne */
      MiniTimer1.start();
      EIFR = _BV(INTF0); /*This call is used to free the interrupt buffer into ATmega328P pin 2*/
      attachInterrupt(digitalPinToInterrupt(MOVE_BTN), moveDown, RISING);
      /* Start the round */
    } else {  /* Update led status during the game phase */
      refreshLed();
    }
  }

}

void refreshLed() {
  noInterrupts();
  int currentLedIndexCopy = currentLedIndex;
  interrupts();
  digitalWrite(leds[0], currentLedIndexCopy == 0 ? HIGH : LOW);
  digitalWrite(leds[1], currentLedIndexCopy == 1 ? HIGH : LOW);
  digitalWrite(leds[2], currentLedIndexCopy == 2 ? HIGH : LOW);
  if (currentLedIndexCopy == LED_BAG_INDEX) { /* Manage fade of bag led when the player goes on it*/
    MiniTimer1.stop();
    analogWrite(BAG_PIN, currIntensity);
    /* Manage intensity of the faded led, if the fade is over start the new round.*/
    currIntensity = currIntensity + fadeAmount; /* First it lights up completely */
    if (currIntensity == 255) { /* Then goes down */
      fadeAmount = -fadeAmount ;
      moveOn = true;
    } else if (currIntensity == 0 && moveOn == true) { /* At the end of the fade it start the next round */
      inRound = false;
      fadeAmount = -fadeAmount;
      roundNumber++;
      Serial.print("Another object in the bag! Count: ");
      Serial.print(roundNumber);
      Serial.println(" objects");
    }
    delayMicroseconds(15000);
  } else if (currentLedIndexCopy == LED_LOSE_INDEX) { /* Manage the red led when the player goes on it */
    MiniTimer1.stop();
    hasLose = true;
  } else {
    delayMicroseconds(200); /* Wait to refresh */
  }
}

void interruptLose() {
  MiniTimer1.stop();
  hasLose = true;
}

void moveDown() {
  long currentTime = millis();
  if (currentTime - prevTimeMoveDown > DEBOUNCE_DELAY) {
    if (currentLedIndex != LED_LOSE_INDEX) {
      currentLedIndex++;
      prevTimeMoveDown = currentTime;
    }
  }
}

void startGame() {
  long currentTime = millis();
  if (currentTime - prevTimeStart > DEBOUNCE_DELAY) {
    Serial.println("Go");
    inGame = true;
    prevTimeStart = currentTime;
  }
}

void moveLed() {
  digitalWrite(leds[ledIndexOfBlinking % 3], LOW);
  ledIndexOfBlinking += direction;
  digitalWrite(leds[ledIndexOfBlinking % 3], HIGH);
  if (ledIndexOfBlinking == 2 || ledIndexOfBlinking == 0) {
    direction *= -1;
  }
}

void readDifficulty() {
  /* Map analog values on a range of eight value (from 1 to 8) */
  int analogValue = analogRead(POTENTIOMETER_PIN);
  int temp = analogValue % 128 ? analogValue / 128 + 1 : analogValue / 128;
  temp += (temp == 0 ? 1 : 0);
  if (temp != difficulty) {
    difficulty = temp;
    Serial.print("Difficoltà: ");
    Serial.println(difficulty);
  }
}

void calculatePeriod() {
  if (roundNumber == 0) { /* If it's the first round it creates a time based on difficulty chosen by the user */
    actualPeriod = 100000 * difficulty;
    actualPeriod = DEFAULT_INTERRUPT_PERIOD - actualPeriod;
  } else { /* If it's not the first round, it removes 1/8 by the previus time for each round */
    long delta = actualPeriod / 8;
    actualPeriod = actualPeriod - delta;
    if (actualPeriod <= 0) {
      actualPeriod  = 0;
    }
  }
}
