#ifndef __STATE__
#define __STATE__
#include "Arduino.h" 

#define PESO_LIMITE_DEL_CESTINO 100
#define AVAILABLE "Available"
#define NOT_AVAILABLE "Not Available"

class State {
    private:
        String actualState;
        int dumpsterActualWeight;
    
    public:
        State();
        String getState();
        int getDepositWeight();
        void addWeight(int weight);
        void setDumpsterState(String newState);

};

#endif
