#include "State.h"

State::State(){
  this->coverStatus = CLOSE;
}

CoverState State::getCoverStatus(){
  return this->coverStatus;
}

void State::setCoverStatus(CoverState value){
  this->coverStatus = value;
}

TrashType State::getTypeOfTrash(){
  return this->typeOfTrash;
}

void State::setTypeOfTrash(TrashType value){
  this->typeOfTrash = value;
}

unsigned long State::getStartTime(){
  return this->timeStartCountDown;
}

void State::setStartTime(unsigned long time){
  /* Memorizzo un timestamp e salvo, in countDownValue, il tempo concesso */
  this->timeStartCountDown = time;
  this->countDownValue = TEMPO_CONCESSO_A_PRESCINDERE;
}

void State::addTimeToCountDown(){
  this->countDownValue += TEMPO_AGGIUNTO_SE_RICHIESTO;
}

int State::getCountDownValue(){
  return this->countDownValue;
}

void State::setTimeExpired(bool value){
  this->timeExpired = value;
}

bool State::isTimeExpired(){
  return this->timeExpired;
}
