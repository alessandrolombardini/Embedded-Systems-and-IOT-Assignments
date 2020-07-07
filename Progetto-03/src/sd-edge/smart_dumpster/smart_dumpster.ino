/* Project Group: Baiardi Martina, 
                  Giachin Marco, 
                  Lombardini Alessandro */

#include "Led.h"
#include "State.h"
#include "Wifi.h"
#include "Potentiometer.h"
#include <ESP8266WebServer.h>

#define PORTA 5000

#define GREEN_LED_PIN 5   /* D1 -> GPIO5 */
#define RED_LED_PIN 4     /* D2 -> GPIO4 */
#define POT_PIN A0

State * state;
Led * redLed;
Led * greenLed;
Wifi * wifi;
Potentiometer * pot;
String lastState; /* Questa variabile consente di catturare il cambio di stato del Dumpster da available a not available, in questo modo viene 
                     segnalato al Servive */ 

/* HTTP rest server */
ESP8266WebServer httpRestServer(PORTA);

void setup(){
    Serial.begin(115200);
    state = new State();
    wifi = new Wifi();
    redLed = new Led(RED_LED_PIN);      /* LED rosso */
    greenLed = new Led(GREEN_LED_PIN);  /* LED verde */
    pot = new Potentiometer(POT_PIN);   /* Potenziometro */

    /* Di default il sistema si presenta disponibile e senza contenuto */
    greenLed->switchOn(); 
    lastState = AVAILABLE; 
    
    /* Definizione delle APi messe a disposizione e associazione dei metodi per gestirle */
    httpRestServer.on("/api/isAvailable", HTTP_POST, availReqHandler);         /* API: richiesta dello stato attuale del DumpsterEdge (ESP) */
    httpRestServer.on("/api/setNotAvailable", HTTP_POST, setNotAvailHandler);  /* API: forzatura a stato NOT_AVAILABLE */ 
    httpRestServer.on("/api/setAvailable", HTTP_POST, setAvailHandler);        /* API: forzatura a stato NOT_AVAILABLE */
    httpRestServer.on("/api/currentWeight", HTTP_POST, currWeightHandler);     /* API: richiesta del peso dell'oggetto depositato */
    httpRestServer.begin();
    Serial.println("HTTP REST Server Started");
}

void loop(){
    /* Verifico se ci sono chiamate pendenti */ 
    httpRestServer.handleClient();

    /* Mantengo aggiornati i LED */
    if (state->getState() == AVAILABLE){
        greenLed->switchOn();
        redLed->switchOff();
    } else {
        greenLed->switchOff();
        redLed->switchOn();
    }
}

/* Handle dell'API: /api/isAvailable */
void availReqHandler(){
    /* Ricevo il messaggio e mando un 200 OK alla sorgente per segnalare che sia arrivato */
    Serial.println("Received request: /api/isAvailable");
    httpRestServer.send(200);
    /* Ora devo mandare la risposta (se la connessione è disponibile) */
    if (WiFi.status()== WL_CONNECTED){   
        /* Messaggio contentente se il dumpster sia AVAILABLE */
        bool message = state->getState() == AVAILABLE ? true : false;
        /* Invio del messaggio */
        Serial.print("Sending "+ state->getState() + " ... ");
        int code = wifi->sendData("isAvailable", (String) message);
        /* Pubblico in console se il messaggio è arrivato a destinazione */
        Serial.println(code == 200 ? "(received)" : "(not received)");   
    } else { 
        Serial.println("Error in WiFi connection.");   
    }
}

/* Handle dell'API: /api/setNotAvailable */
void setNotAvailHandler (){
    /* Pubblico in console un resoconto */
    Serial.print("Received request: Force NOT AVAILABLE  ->  ");
    Serial.print("Dumpster deposit weight: ");
    Serial.print(state->getDepositWeight());
    Serial.print("/");
    Serial.println(PESO_LIMITE_DEL_CESTINO);
    /* Applico le modifiche */
    httpRestServer.send(200);
    state->setDumpsterState(NOT_AVAILABLE);
}

/* Handle dell'API: /api/setAvailable */
void  setAvailHandler(){
    /* Pubblico in console un resoconto */
    Serial.print("Received request: Force AVAILABLE  ->  ");
    Serial.print("Dumpster deposit weight: ");
    Serial.print(state->getDepositWeight());
    Serial.print("/");
    Serial.println(PESO_LIMITE_DEL_CESTINO);
    /* Applico le modifiche */
    httpRestServer.send(200);
    state->setDumpsterState(AVAILABLE);
    lastState = AVAILABLE;
}

/* Handle dell'API: /api/currentWeight */
void currWeightHandler(){
    /* Ricevo il messaggio e mando un 200 OK alla sorgente per segnalare che sia arrivato */
    Serial.println("Received request: Weight.");
    httpRestServer.send(200);
    /* Ottengo il peso del potenziometro e lo sommo alla quantità attuale */
    int weight = pot->getValue();
    state->addWeight(weight);
    /* Pubblico in console l'aggiunta del peso */
    Serial.print("Dumpster deposit weight: ");
    Serial.print(state->getDepositWeight());
    Serial.print("/");
    Serial.println(PESO_LIMITE_DEL_CESTINO);
    /* Se lo stato è cambiato (ovvero il peso ha superato la soglia) invio un messaggio al DumpsterService */
    if (lastState == AVAILABLE && state->getState() == NOT_AVAILABLE){
        wifi->sendData("isAvailable", "false");
        lastState = NOT_AVAILABLE;
    }
    /* Ora devo inviare la risposta (se la connessione è disponibile) */
    if (WiFi.status()== WL_CONNECTED){   
        /* Invio del messaggio */
        Serial.print("Sending ");
        Serial.print(weight);
        Serial.print(" of weight... ");
        int code = wifi->sendData("currentWeight", (String) weight);
        /* Pubblico in console se il messaggio è arrivato a destinazione */
        Serial.println(code == 200 ? "(received)" : "(not received)");
    } else { 
        Serial.println("Error in WiFi connection.");   
    }
}
