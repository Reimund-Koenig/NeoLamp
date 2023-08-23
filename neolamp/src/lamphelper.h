#ifndef LAMPHELPER
#define LAMPHELPER
#include "clocktime.h"
#include <Arduino.h>

class LampHelper {
  public:
    LampHelper();
    int get_state(Clocktime current_time, Clocktime t1, int m1, Clocktime t2,
                  int m2, Clocktime t3, int m3);
    void set_none_sleeping_delay(unsigned long wait,
                                 unsigned long *sleepUntilTime);
    bool is_sleeping(unsigned long sleepUntilTime);

  private:
    Clocktime get_biggest_time(Clocktime t1, Clocktime t2);
    Clocktime get_biggest_time(Clocktime t1, Clocktime t2, Clocktime t3);
    bool is_t1_greater_or_euqal_t2(Clocktime t1, Clocktime t2);
    bool is_time_equal(Clocktime t1, Clocktime t2);
    int get_state_helper(Clocktime current_time, Clocktime big_t, int m1,
                         Clocktime t2, int m2, Clocktime t3, int m3);
};
#endif // LAMPHELPER
