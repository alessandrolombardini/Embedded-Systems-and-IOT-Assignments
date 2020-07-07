#include "ManageBTTask.h"

ManageBTTask::ManageBTTask(int TXPin, int RXPin, String btName){
    service = new MsgServiceBT(TXPin, RXPin);
    service->init();
    /* Rendo raggiungibile il modulo Bluetooth (Name: dumpster) */
    service->sendMsg(Msg("AT")); 
    service->sendMsg(Msg("AT+NAME"+btName)); 
    delay(100);
}

void ManageBTTask::init(int period, State * state){
    Task::init(period, state);
    this->state = state;
}

void ManageBTTask::tick(){
    /* Gestione dei messaggi provenienti dall'APP */  
    if (service->isMsgAvailable()) {
      Msg* msg = service->receiveMsg();
      if(msg != NULL){ /* Se ritorna NULL implica che il messaggio non è ancora arrivato tutto */
        processBTMsg(msg->getContent());
        delete msg;
      }
    }
    /* Gestione fine del tempo di deposito: invio messaggio all'APP */
    if(state->isTimeExpired()){
      state->setTimeExpired(false);
      sendEndOfTime(); 
    }
}


/************************************************************************************************************/
/************************* Metodo per la gestione dei messaggi provenienti dall'APP *************************/
/************************************************************************************************************/

void ManageBTTask::processBTMsg(String message){
  /* Estrazione delle informazioni contenute nel messaggio */
  const int CAPACITY = JSON_OBJECT_SIZE(15);
  StaticJsonDocument<CAPACITY> doc;  
  DeserializationError err = deserializeJson(doc, message); 
  if (err) {
    /* Il parse del JSON non è andato a buon fine */
    Serial.print("Message received was wrong");
  } else {
    String typeOfMessage = doc["messageType"].as<char*>();
    /* Vengono qui gestite le tre tipologie di messaggio che l'APP è in grado di mandare */
    if(typeOfMessage=="typeOfTrash"){                 /* Gestione messaggio: tipologia di rifiuto */
        String value = doc["value"].as<char*>(); 
        /* Estrazione del token */
        String token = "";
        for(int i = 49; message[i] != '}' && i < message.length() - 1; i++){
          token += message[i];
        }
        if(token == ""){ /* Token non presente */
          sendResponseToTypeOfTrashNotOK();
        } else {
          if(value == "A"){
              this->state->setTypeOfTrash(TRASH_A);
          } else if (value == "B"){
              this->state->setTypeOfTrash(TRASH_B);
          } else if (value == "C"){
              this->state->setTypeOfTrash(TRASH_C);
          }
          this->state->setCoverStatus(OPEN);
          this->state->setStartTime(millis());
          sendResponseToTypeOfTrash();
        }
    } else if(typeOfMessage=="finishDeposit"){        /* Gestione messaggio: è stato terminato il deposito prima del tempo */
      this->state->setCoverStatus(CLOSE);
      this->sendEndOfTime();
    } else if(typeOfMessage=="requestOfTime"){        /* Gestione messaggio: aggiunta di tempo per il deposito */ 
      this->state->addTimeToCountDown();
      sendResponseToRequestOfTime();
    }
  }
}

/*****************************************************************************************/
/******************* Metodi che gestiscono l'invio di messaggi all'APP *******************/
/*****************************************************************************************/

/* Risposta alla selezione del tipo di rifiuto */
void ManageBTTask::sendResponseToTypeOfTrash(){
  const int capacity = JSON_OBJECT_SIZE(3);
  StaticJsonDocument<capacity> doc;
  doc["messageType"] = "typeOfTrash";
  doc["value"] = "OK";
  doc["time"] = TEMPO_CONCESSO_A_PRESCINDERE;
  service->sendMsg(Msg(doc.as<String>())); 
}

/* Risposta negativa alla selezione del tipo di rifiuto */
void ManageBTTask::sendResponseToTypeOfTrashNotOK(){
  const int capacity = JSON_OBJECT_SIZE(3);
  StaticJsonDocument<capacity> doc;
  doc["messageType"] = "typeOfTrash";
  doc["value"] = "NOT OK";
  service->sendMsg(Msg(doc.as<String>())); 
}

/* Risposta alla richiesta di più tempo */
void ManageBTTask::sendResponseToRequestOfTime(){
  const int capacity = JSON_OBJECT_SIZE(3);
  StaticJsonDocument<capacity> doc;
  doc["messageType"] = "requestOfTime";
  doc["value"] = "OK";
  doc["time"] = TEMPO_AGGIUNTO_SE_RICHIESTO;
  service->sendMsg(Msg(doc.as<String>())); 
}

/* Invio di riassunto del deposito - indica la fine del deposito */
void ManageBTTask::sendEndOfTime(){
  const int capacity = JSON_OBJECT_SIZE(4);
  StaticJsonDocument<capacity> doc;
  doc["messageType"] = "endOfConnection";
  doc["value"] = "OK";
  if (this->state->getTypeOfTrash() == TRASH_A){
    doc["type"] = "A";
  } else if (this->state->getTypeOfTrash() == TRASH_B){
    doc["type"] = "B";
  } else if (this->state->getTypeOfTrash() == TRASH_C){
    doc["type"] = "C";
  }
  service->sendMsg(Msg(doc.as<String>())); 
}

/*****************************************************************************************/
/*****************************************************************************************/
/*****************************************************************************************/
