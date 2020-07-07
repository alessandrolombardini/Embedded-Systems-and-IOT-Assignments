#include "Arduino.h"
#include "MsgServiceBT.h"


MsgServiceBT::MsgServiceBT(int txPin, int rxPin){
  channel = new SoftwareSerial(txPin, rxPin);
}

void MsgServiceBT::init(){
  content.reserve(256);
  channel->begin(9600);
}

bool MsgServiceBT::sendMsg(Msg msg){
  channel->println(msg.getContent());  
}

bool MsgServiceBT::isMsgAvailable(){
  return channel->available();
}

Msg* MsgServiceBT::receiveMsg(){
  while (channel->available()) {
    char ch = (char) channel->read();
    /* I carattei \n vengono scartati: il termine della ricezione di un messaggio è data dal carettere }, in quanto messaggio JSON */
    if(ch != '\n'){
      content += ch;
      if (ch == '}'){
        Msg* msg = new Msg(content);  
        content = "";
        return msg;    
      } 
    }
  }
  /* Se il messaggio non è terminato ritorno NULL: in questo modo è possibile restituire tutto il messaggio completo.
     Le varie letture vanno a salvarsi nella variabile 'content', che viene svuotata solo al termine dell ricezione di tutto il messaggio */
  return NULL;  
}
