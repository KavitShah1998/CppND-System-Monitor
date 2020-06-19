#include <string>

#include "format.h"

using std::string;


// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    int hrs{0}, mins{0}, secs{0};
    long temp{seconds};

    hrs = temp/3600;    
    temp = temp%3600;
    mins = temp/60;
    secs = temp%60;

    std::string time{""};
    time = std::to_string(hrs) + ":" + std::to_string(mins) + ":" + std::to_string(secs);

    return time; 
}