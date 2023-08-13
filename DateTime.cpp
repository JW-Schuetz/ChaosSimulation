
#include "DateTime.h"


void DateTime::setTime()
{
    time_t t = time( NULL );
    localtime_s( &actTime, &t );

    datetime = time2string();
}

string DateTime::getDateTime()
{
  return datetime;
}

string DateTime::getDate()
{
    size_t ndx = datetime.find_first_of( ' ' );
    return string( datetime.begin(), datetime.begin() + ndx );
}

string DateTime::getTime()
{
    size_t ndx = datetime.find_first_of( ' ' );
    return string( datetime.begin() + 1 + ndx, datetime.end() );
}

bool DateTime::dayChange( const DateTime & dt )
{
  bool ret {};
  if( actTime.tm_mday != dt.actTime.tm_mday ) ret = true;

  return ret;
}

string DateTime::time2string()
{
#define NALLOC 24   // maximal needed buffer size should be 19: "%02d.%02d.%04d %02d:%02d:%02d"

  char * cbuff = (char *)malloc( NALLOC );

  int ret = snprintf( cbuff, NALLOC, "%02d.%02d.%04d %02d:%02d:%02d",
    actTime.tm_mday, actTime.tm_mon + 1, actTime.tm_year + 1900,
    actTime.tm_hour, actTime.tm_min, actTime.tm_sec );

  string time( cbuff, 0, ret );

  free( cbuff );

  return time;
}