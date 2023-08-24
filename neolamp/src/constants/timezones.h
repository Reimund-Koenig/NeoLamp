
// Now we can set the real timezone
// https://randomnerdtutorials.com/esp32-ntp-timezones-daylight-saving/
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

const char *timezones[][2] = {{"Berlin", "CET-1CEST,M3.5.0,M10.5.0/3"},
                              {"London", "GMT0BST,M3.5.0/1,M10.5.0"},
                              {"Paris", "CET-1CEST,M3.5.0,M10.5.0/3"},
                              {"Rome", "CET-1CEST,M3.5.0,M10.5.0/3"},
                              {"Kiev", "EET-2EEST,M3.5.0/3,M10.5.0/4"},
                              {"Istanbul", "<+03>-3"},
                              {"Zurich", "CET-1CEST,M3.5.0,M10.5.0/3"}};

const int sizeof_timezones = sizeof(timezones) / sizeof(timezones[0]);
