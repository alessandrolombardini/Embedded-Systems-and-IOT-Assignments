#include "ManageCountDownTask.h"

ManageCountDownTask::ManageCountDownTask(){}

void ManageCountDownTask::init(int period, State * state){
    Task::init(period, state);
    this->state = state;
}

void ManageCountDownTask::tick(){
    if(state->getCoverStatus() == OPEN){
        unsigned long actualTime = millis();
        unsigned long differenceNow = actualTime - state->getStartTime();
        unsigned long differenceToReach = state->getCountDownValue() * 1000L; 
        if(differenceToReach < differenceNow){ 
            /* Tempo scaduto */
            state->setCoverStatus(CLOSE);
            state->setTimeExpired(true);
        } 
    }
}