#pragma once

#include <string>
#include <ctime>


using std::string;


// handle actual time
class DateTime
{
public:
    void setTime();                     // set actual time
    string getDate();                   // get date as string
    string getTime();                   // get time as string
    string getDateTime();               // get date and time as string
    bool dayChange( const DateTime & ); // change of day happend

private:
    struct tm actTime = {};
    string datetime = {};

    string time2string();       // convert time to string
};