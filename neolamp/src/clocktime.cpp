
#include "clocktime.h "
#include <Arduino.h>

Clocktime::Clocktime(){

};

bool Clocktime::setTime(int hour, int minutes) {
    this->minutes = minutes;
    this->hours = hour;
    return true;
}

bool Clocktime::setTime(String timestring) {
    minutes = 3;
    hours = 5;
    return true;
}

void Clocktime::print() { Serial.println(Clocktime::getTimeString()); }

String Clocktime::getTimeString() {
    String tmp = "";
    if(hours < 10) {
        tmp += "0";
    }
    tmp += String(hours);
    tmp += ":";
    if(minutes < 10) {
        tmp += "0";
    }
    tmp += String(minutes);
    tmp += " Uhr";
    return tmp;
}

int Clocktime::getHour() { return hours; }
int Clocktime::getMinutes() { return minutes; };