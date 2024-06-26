
#include "clocktime.h "

Clocktime::Clocktime(){

};

bool Clocktime::setTime(int hour, int minutes, int seconds) {
    this->hours = hour;
    this->minutes = minutes;
    this->seconds = seconds;
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

boolean Clocktime::isValidNumber(String str) {
    for(byte i = 0; i < str.length(); i++) {
        if(!isDigit(str.charAt(i))) { return false; }
    }
    return true;
}

bool Clocktime::setTime(String timestring) {
    if(timestring.length() < 3) { return false; }
    String timestring_minutes = getValue(timestring, ':', 1);
    String timestring_hours = getValue(timestring, ':', 0);
    if(timestring_hours == "" || timestring_minutes == "") { return false; }
    if(!isValidNumber(timestring_minutes)) { return false; }
    if(!isValidNumber(timestring_hours)) { return false; }
    int tmp_min = timestring_minutes.toInt();
    int tmp_hour = timestring_hours.toInt();
    if(tmp_hour < 0 || tmp_hour > 24) { return false; }
    if(tmp_min < 0 || tmp_min > 60) { return false; }
    this->minutes = tmp_min;
    this->hours = tmp_hour;
    if(timestring.length() > 5) {
        // with seconds
        String timestring_seconds = getValue(timestring, ':', 2);
        if(!isValidNumber(timestring_seconds)) { return false; }
        int tmp_sec = timestring_seconds.toInt();
        if(tmp_sec < 0 || tmp_sec > 60) { return false; }
        this->seconds = tmp_sec;
    }
    return true;
}

void Clocktime::print() { Serial.println(Clocktime::getTimeString()); }

String Clocktime::getTimeString() {
    String tmp = "";
    if(hours < 10) { tmp += "0"; }
    tmp += String(hours);
    tmp += ":";
    if(minutes < 10) { tmp += "0"; }
    tmp += String(minutes);
    tmp += " Uhr";
    return tmp;
}

int Clocktime::getHour() { return hours; }
int Clocktime::getMinutes() { return minutes; };
int Clocktime::getSeconds() { return seconds; };
