#include <string>

#include "format.h"

using std::string;
using std::to_string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::AppendZero(const long number) {
    if (number < 10) {
        return "0" + to_string(number);
    }
    return to_string(number);
}


string Format::ElapsedTime(const long seconds) { 
    const int seconds_in_minutes = 60;
    const int seconds_in_hours = 3600;
    const long hours = seconds / seconds_in_hours;
    const int minutes = (seconds % seconds_in_hours) / seconds_in_minutes;
    const int remaining_seconds = seconds % seconds_in_minutes; 
    return AppendZero(hours) + ":" + AppendZero(minutes) + ":" +
           AppendZero(remaining_seconds);
}