#include "Wifi.h"

Wifi::Wifi(){
    /* ---------- Connessione alla rete Wi-Fi ---------- */
    if (initWifi() == WL_CONNECTED) {
        Serial.print("Connetted to ");
        Serial.print(WIFI_NAME);
        Serial.print("--- IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.print("Error connecting to: ");
        Serial.println(WIFI_NAME);
    }
}

int Wifi::initWifi() {
    Serial.print("Trying to connect to WiFi...");
    /* Effettuo un primo tentativo di connessione */
    int retries = 0;
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_NAME, WIFI_PWD);
    
    /* Se la connessione non è stata instaurata, comincio con i successivi tentativi */
    while ((WiFi.status() != WL_CONNECTED) && (retries < TENTATIVI)) {
        /* L'istruzione per richiede la connessione è una soltanto: Wifi.begin().
           Il concetto di tentativo è inteso come mettere il pausa l'ESP per ATTESA_TENTATIVO millisecondi 
           prima di continuare con la richiesta di connessione */
        delay(ATTESA_TENTATIVO);
        retries++;
        Serial.print(".");
    }
    Serial.println("");
    return WiFi.status(); /* Ritorno lo stato finale della connessione */
} 

/* Funzione che permette di inviare dati ad un destinatario */
int Wifi::sendData(String api, String value){ 
   HTTPClient http;
   /* Definisco destinatario e messaggio */
   String address = ADDR;                   /* ADDR nel nostro caso è definito come l'IP del Dumpster Service (PC Server), ovvero il nostro unico destinatario */
   address += api;                          /* Specifico l'api da contattare */
   String msg = "{\"Message\": \"" + value + "\"}";
   /* Stampo il resoconto sulla console di ciò che è stato inviato*/
   Serial.print("Send to -> ");
   Serial.print(address);
   Serial.print("(Message -> ");
   Serial.print(msg);
   Serial.println(""); 
   /* Ritorno il codice HTTP */
   http.begin(address);      
   http.addHeader("Content-Type", "application/json");     
   int retCode = http.POST(msg);   
   http.end();       
   return retCode;
}
