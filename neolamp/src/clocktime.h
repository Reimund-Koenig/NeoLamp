#ifndef CLOCKTIME
#define CLOCKTIME
#include <Arduino.h>

class Clocktime {
  public:
    Clocktime();
    void print();
    String getTimeString();
    bool setTime(String timestring);
    bool setTime(int hour, int minutes, int seconds);
    int getHour();
    int getMinutes();
    int getSeconds();

  private:
    int seconds;
    int minutes;
    int hours;
    boolean isValidNumber(String str);
};
#endif // CLOCKTIME
