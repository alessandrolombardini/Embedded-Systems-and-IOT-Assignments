#include "ManageLedTask.h"

ManageLedTask::ManageLedTask(int pin_A, int pin_B, int pin_C){
  this->pin_A = pin_A;
  this->pin_B = pin_B;
  this->pin_C = pin_C;
}

void ManageLedTask::init(int period, State * state){
  Task::init(period, state);
  this->led_A = new Led(pin_A);
  this->led_B = new Led(pin_B);
  this->led_C = new Led(pin_C);
  this->state = state;
}

void ManageLedTask::tick(){
  /* Se il portellone è aperto mantengo acceso uno dei tre led */
  if(state->getCoverStatus() == OPEN){ 
    if(state->getTypeOfTrash() == TRASH_A){
        led_A->switchOn();
    } else if (state->getTypeOfTrash() == TRASH_B){
        led_B->switchOn();
    } else if (state->getTypeOfTrash() == TRASH_C){
        led_C->switchOn();
    }
  } else {
        led_A->switchOff();
        led_B->switchOff();
        led_C->switchOff();
  }
}
