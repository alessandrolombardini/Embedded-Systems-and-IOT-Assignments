#ifndef __STATE__
#define __STATE__

#include "Arduino.h"

/* Stati del sistema */
typedef enum { OPEN,  /* Il cestino è aperto */ 
               CLOSE  /* Il cestino è chiuso */
              } CoverState;

/* Tipologie di rifiuto */
typedef enum {TRASH_A, 
              TRASH_B,
              TRASH_C
              } TrashType; 

#define TEMPO_CONCESSO_A_PRESCINDERE 90   /* Tempo base concesso per il deposito */
#define TEMPO_AGGIUNTO_SE_RICHIESTO 30    /* Tempo aggiunto ad ogni richiesta dell'utente di aggiunta di tempo */

class State {
  private:
    CoverState coverStatus;               /* Stato del cassonetto: OPEN o CLOSE */
    TrashType typeOfTrash;                /* Tipo di rifiuto che si vuole caricare: TRASH_A, TRASH_B, TRASH_C (utile se il cassonetto è aperto) */
    unsigned long timeStartCountDown;     /* Riferimento temporale che indica l'istante in cui il cassonetto è stato aperto */
    int countDownValue;                   /* Valore che indica quanto il cassonetto deve rimanere aperto in termini di secondi: può essere aumentato se l'utente richiede più tempo */
    bool timeExpired = false;             /* Il task ManageCountDownTask segnala al bluetooth che il tempo è terminato mediante questa variabile */

  public:
    State();
    void setCoverStatus(CoverState value);    /* Imposto OPEN o CLOSE: i due possibili stati del cassonetto */
    CoverState getCoverStatus();  
    void setTypeOfTrash(TrashType value);     /* Imposto TRASH_A, TRASH_B, TRASH_C: le tre possibili tipologie di rifiuto */
    TrashType getTypeOfTrash();
    void setStartTime(unsigned long time);    /* Imposta l'istante in cui abbiamo ricevuto la tipologia di rifiuto: inoltre viene inserito il tempo di count down (variabile 'countdown') */
    unsigned long getStartTime();
    void addTimeToCountDown();                /* Aggiunge tempo alla variabile 'countdown' */
    int getCountDownValue();                  /* Ottiene il tempo concesso in totale, quello iniziale più le eventuali richieste. E' la variabile 'countdown' */
    void setTimeExpired(bool value);          /* Imposta se il tempo è terminato o no */
    bool isTimeExpired();
};

#endif
