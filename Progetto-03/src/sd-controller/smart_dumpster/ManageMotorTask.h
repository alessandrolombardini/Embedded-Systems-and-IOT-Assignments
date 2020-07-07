#ifndef __MANAGEMOTORTASK__
#define __MANAGEMOTORTASK__

#include "ServoMotor.h"
#include "State.h"
#include "Task.h"

class ManageMotorTask:public Task {
  private:
    ServoMotor * motor;      
    int pin;
    CoverState lastState;
    State * state;                   

  public:
    ManageMotorTask(int pin);
    void init(int period, State * state);
    void tick();   
    void openCover();
    void closeCover();
};

#endif
