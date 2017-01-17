/*
 * logger.cpp
 *
 *  Created on: Jan 16, 2017
 *      Author: dmitry
 */

#include "logger.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

bool debugEnabled;

void print_now() {
    // Get current time from the clock, using microseconds resolution
    const boost::posix_time::ptime now =
        boost::posix_time::microsec_clock::local_time();

    // Get the time offset in current day
    const boost::posix_time::time_duration td = now.time_of_day();

    const long hours        = td.hours();
    const long minutes      = td.minutes();
    const long seconds      = td.seconds();
    const long milliseconds = td.total_milliseconds() -
                              ((hours * 3600 + minutes * 60 + seconds) * 1000);

    char buf[40];
    sprintf(buf, "%02ld:%02ld:%02ld,%03ld",
        hours, minutes, seconds, milliseconds);
    std::cout << buf;
}
