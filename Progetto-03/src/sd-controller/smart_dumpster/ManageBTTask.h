#ifndef __MANAGEBTTASK__
#define __MANAGEBTTASK__

#include "SoftwareSerial.h"
#include "MsgServiceBT.h"
#include <ArduinoJson.h>
#include "Task.h"
#include "State.h"

class ManageBTTask: public Task{
    private:
        State * state;
        MsgServiceBT * service; 
        void processBTMsg(String message);
        void sendResponseToRequestOfTime();
        void sendResponseToTypeOfTrash();
        void sendResponseToTypeOfTrashNotOK();
        void sendEndOfTime();

    public:
        ManageBTTask(int TXPin, int RXPin, String btName);
        void init(int period, State * state);
        void tick();
};

#endif
