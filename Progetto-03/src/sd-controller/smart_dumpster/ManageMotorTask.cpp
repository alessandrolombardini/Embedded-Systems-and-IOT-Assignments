#include "ManageMotorTask.h"

ManageMotorTask::ManageMotorTask(int pin){
  this->pin = pin;
}

void ManageMotorTask::init(int period, State * state){
  Task::init(period, state);
  motor = new ServoMotor(pin);
  this->state = state;
  lastState = state->getCoverStatus();
  /* Porto il motore nella posizione iniziale */
  motor->on();
  for (int i = 0; i < 120; i++) {
    motor->setPosition(i);         
    delay(5);    
  }
}

void ManageMotorTask::openCover(){
    int pos = 0;
    int delta = 1;
    for (int i = 0; i < 120; i++) {
        motor->setPosition(pos);
        pos += delta;         
    }
}

void ManageMotorTask::closeCover(){
    int pos = 120;
    int delta = -1;
    for (int i = 0; i < 120; i++) {
        motor->setPosition(pos);
        pos += delta;
    }
}

void ManageMotorTask::tick(){
  motor->on();
  CoverState actualState = state->getCoverStatus();
  if (actualState != lastState){
    if (actualState == CLOSE){
      openCover();
    } else {
      closeCover();
    }
    delay(100); /* Questo delay consente al motore di spostarci fino al punto stabilito */
    lastState = actualState;
  }
  motor->off(); /* Se il motore non viene spendo tende ad essere soggetto ad impulsi non desiderati */
}
