#include "State.h"


State::State(){
    this->actualState = AVAILABLE;
    this->dumpsterActualWeight = 0;
}

String State::getState(){
    return this->actualState;
}
        
int State::getDepositWeight(){
    return this->dumpsterActualWeight;
}
        
void State::addWeight(int weight){
    this->dumpsterActualWeight += weight;
    if (dumpsterActualWeight >= PESO_LIMITE_DEL_CESTINO){
        setDumpsterState(NOT_AVAILABLE);
    }
}
        
void State::setDumpsterState(String newState){
    if (newState == AVAILABLE || newState == NOT_AVAILABLE){
        this->actualState = newState;
        /* Se prima non era dispobile e ora si, significa che il cestino è stato liberato, quindi azzero il peso */
        if (newState == AVAILABLE){ 
            this->dumpsterActualWeight = 0;
        }
    }
}
