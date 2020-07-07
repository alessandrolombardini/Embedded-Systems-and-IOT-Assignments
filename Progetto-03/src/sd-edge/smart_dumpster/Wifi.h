#ifndef __WIFI__
#define __WIFI__

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define ATTESA_TENTATIVO 500  /* Attesa tra i tentativi di connessione al wi-fi */ 
#define TENTATIVI 50          /* Numero di tentativi massimi da effettuare */ 

/* Wifi network name */
#define WIFI_NAME "Telecom-89614151"

/* WPA2 PSK password */
#define WIFI_PWD "pasticceriacaffetteriafabbri"

/* Dumpster Service (PC Server) IP address */ 
#define ADDR "http://bb0acec9.ngrok.io/api/"

class Wifi {
    private:
        int initWifi();

    public:
        Wifi();
        int sendData(String api, String value);
};

#endif
