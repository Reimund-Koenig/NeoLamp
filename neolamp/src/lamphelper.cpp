
#include "lamphelper.h"
#include <Arduino.h>

LampHelper::LampHelper(){

};

Clocktime LampHelper::get_biggest_time(Clocktime t1, Clocktime t2) {
    if(t1.getHour() > t2.getHour()) { return t1; }
    if(t2.getHour() > t1.getHour()) { return t2; }
    if(t1.getMinutes() > t2.getMinutes()) { return t1; }
    return t2;
}

Clocktime LampHelper::get_biggest_time(Clocktime t1, Clocktime t2,
                                       Clocktime t3) {
    Clocktime tmp = get_biggest_time(t1, t2);
    return get_biggest_time(tmp, t3);
}

bool LampHelper::is_t1_greater_or_euqal_t2(Clocktime t1, Clocktime t2) {
    if(t1.getHour() > t2.getHour()) { return true; }
    if(t1.getHour() < t2.getHour()) { return false; }
    return t1.getMinutes() >= t2.getMinutes();
}

bool LampHelper::is_time_equal(Clocktime t1, Clocktime t2) {
    if(t1.getHour() == t2.getHour() && t1.getMinutes() == t2.getMinutes()) {
        return true;
    }
    return false;
}

int LampHelper::get_state_helper(Clocktime current_time, Clocktime big_t,
                                 int m1, Clocktime t2, int m2, Clocktime t3,
                                 int m3) {
    // big_t1 must be the biggest time
    if(is_t1_greater_or_euqal_t2(current_time, big_t)) { return m1; }
    big_t = get_biggest_time(t2, t3);
    if(is_time_equal(big_t, t2)) {
        // t2 is the second biggest time, t3 the smallest
        if(is_t1_greater_or_euqal_t2(current_time, t2)) { return m2; }
        if(is_t1_greater_or_euqal_t2(current_time, t3)) { return m3; }
    } else {
        // t3 is the second biggest time, t2 the smallest
        if(is_t1_greater_or_euqal_t2(current_time, t3)) { return m3; }
        if(is_t1_greater_or_euqal_t2(current_time, t2)) { return m2; }
    }
    // current time is smaller then all times
    return m1;
}

int LampHelper::get_state(Clocktime current_time, Clocktime t1, int m1,
                          Clocktime t2, int m2, Clocktime t3, int m3) {
    Clocktime big_t = get_biggest_time(t1, t2, t3);
    if(is_time_equal(big_t, t1)) {
        // t1 is the biggest time
        return get_state_helper(current_time, t1, m1, t2, m2, t3, m3);
    } else if(is_time_equal(big_t, t2)) {
        // t2 is the biggest time
        return get_state_helper(current_time, t2, m2, t1, m1, t3, m3);
    } else {
        // t3 is the biggest time
        return get_state_helper(current_time, t3, m3, t2, m2, t1, m1);
    }
}