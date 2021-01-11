#ifndef SYSTEM_INFO_UTIL_HPP__
#define SYSTEM_INFO_UTIL_HPP__

#include <chrono>

namespace System{

double ticks_to_seconds(unsigned long);
double ticks_to_percent(unsigned long, std::chrono::duration<int> const);
unsigned long pages_to_kb(unsigned long);

}

#endif /* SYSTEM_INFO_UTIL_HPP__ */
