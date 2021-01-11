#ifndef SYSTEM_HARDWARE_INFO_HPP__
#define SYSTEM_HARDWARE_INFO_HPP__

#include "system_types.hpp"
#include <chrono>

namespace System{

bool hardware_cpu_usage(Hardware_CPU_Usage& usage) noexcept;
void hardware_cpu_time_percent(CPU_Time<double>&, CPU_Time<unsigned long> const&) noexcept;
bool hardware_cpu_time_percent(CPU_Time<double>&, std::string cpu = "cpu" ) noexcept;
bool hardware_cpu_time_percent(CPU_Time<double>&,
		std::chrono::duration<int>,
		std::string cpu = "cpu") noexcept;
bool hardware_memory_usage(Hardware_Memory_Usage&) noexcept;

}//System

#endif /* SYSTEM_HARDWARE_INFO_HPP__ */
