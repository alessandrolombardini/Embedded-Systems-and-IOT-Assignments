#ifndef __MANAGELEDTASK__
#define __MANAGELEDTASK__

#include "Task.h"
#include "Led.h"
#include "State.h"

class ManageLedTask: public Task {
  private:
    int pin_A;
    int pin_B;
    int pin_C;
    Led* led_A;
    Led* led_B;
    Led* led_C;
    State * state;
  
  public:
    ManageLedTask(int pin_A, int pin_B, int pin_C);
    void init(int period, State * state);
    void tick();
};

#endif
