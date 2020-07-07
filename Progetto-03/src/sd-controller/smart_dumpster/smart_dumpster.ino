/* Project Group: Baiardi Martina, 
                  Giachin Marco, 
                  Lombardini Alessandro */
                  
#include "ManageMotorTask.h"
#include "ManageBTTask.h"
#include "ManageLedTask.h"
#include "ManageCountDownTask.h"
#include "Scheduler.h"
#include "Task.h"

#define SERVO_PIN 10    /* PIN: servo motore */
#define LA_PIN 5        /* PIN: led rifiuto tipo A */
#define LB_PIN 7        /* PIN: led rifiuto tipo B */
#define LC_PIN 8        /* PIN: led rifiuto tipo C */
#define BT_TXD_PIN 2    /* PIN: bluetooth TX */
#define BT_RXD_PIN 3    /* PIN: bluetooth RX */

#define TICK_TIME 200

#define SERVO_TICK 200
#define BLUETOOTH_TICK 200 
#define LED_TICK 200
#define COUNTDOWN_TICK 200

Scheduler * scheduler;
State * state;

void setup() {

  Serial.begin(9600);
  state = new State();
  scheduler = new Scheduler();
  scheduler->init(TICK_TIME);       
  
  /* Task: gestione del countdown */
  Task * CDTask = new ManageCountDownTask();
  CDTask->init(COUNTDOWN_TICK, state);
  scheduler->addTask(CDTask);

  /* Task: gestione del bluetooth */
  Task * BTTask = new ManageBTTask(BT_TXD_PIN, BT_RXD_PIN, "dumpster");
  BTTask->init(BLUETOOTH_TICK, state);
  scheduler->addTask(BTTask);

  /* Task: gestione del motore */
  Task * servoTask = new ManageMotorTask(SERVO_PIN);
  servoTask->init(SERVO_TICK, state);
  scheduler->addTask(servoTask);

  /* Task: gestione dei led relativi al tipo di rifiuto */
  Task * ledTask = new ManageLedTask(LA_PIN, LB_PIN, LC_PIN);
  ledTask->init(LED_TICK, state);
  scheduler->addTask(ledTask);
}

void loop() {
  scheduler->schedule();
}
