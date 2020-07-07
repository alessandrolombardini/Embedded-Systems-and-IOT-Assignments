#ifndef __MANAGECDTASK__
#define __MANAGECDTASK__

#include "Arduino.h"
#include "Task.h"

class ManageCountDownTask: public Task {
  private:
    State * state;

  public:
    ManageCountDownTask();
    void init(int period, State * state);
    void tick();
};

#endif
