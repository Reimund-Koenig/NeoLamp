
#include "clocktime.h "
#include <Arduino.h>

Clocktime::Clocktime(){

};

bool Clocktime::setTime(int hour, int minutes) {
    this->minutes = minutes;
    this->hours = hour;
    return true;
}

String getValue(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for(int i = 0; i <= maxIndex && found <= index; i++) {
        if(data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

bool Clocktime::setTime(String timestring) {
    String timestring_minutes = getValue(timestring, ':', 0);
    String timestring_hours = getValue(timestring, ':', 1);
    Serial.print("Value Minutes: ");
    Serial.print(timestring_minutes);
    Serial.print("Value Hours: ");
    Serial.println(timestring_hours);
    int tmp_min = timestring_minutes.toInt();
    int tmp_hour = timestring_hours.toInt();
    Serial.print("INT Minutes: ");
    Serial.print(tmp_min);
    Serial.print("INT Hours: ");
    Serial.println(tmp_hour);
    if(tmp_hour < 0 || tmp_hour > 24) {
        return false;
    }
    if(tmp_min < 0 || tmp_min > 60) {
        return false;
    }
    this->minutes = tmp_min;
    this->hours = tmp_hour;
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