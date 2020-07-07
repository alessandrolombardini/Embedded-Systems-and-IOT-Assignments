#include "Potentiometer.h"
#include "Arduino.h"

Potentiometer::Potentiometer(int pin){
    this->pin = pin;
    pinMode(pin,INPUT);
}

int Potentiometer::getValue(){
    /* Il valore può essere 0, 10, 20, ..., 70 */
    int analogValue = analogRead(pin);
    int temp = analogValue % 128 ? analogValue/128 + 1 : analogValue/128; 
    temp += temp == 0 ? 1 : 0;
    return (temp*10) - 10; 
}
