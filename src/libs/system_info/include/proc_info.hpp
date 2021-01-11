#ifndef SYSTEM_PROC_INFO_HPP__
#define SYSTEM_PROC_INFO_HPP__

#include <unistd.h>
#include <chrono>
#include "system_types.hpp"

namespace System{

bool process_info(Process_Info&) noexcept;
bool process_info(pid_t, Process_Info&) noexcept;

bool uptime(Uptime&) noexcept;

bool process_uptime(Process_Info&, double&) noexcept;
bool process_uptime(pid_t, double&) noexcept;
bool process_uptime(double&) noexcept;

/**
 * Overall percent usage
 */
unsigned long cpu_usage(bool use_child_time = true) noexcept;
unsigned long cpu_usage(Process_Info&, bool use_child_time = true) noexcept;
unsigned long cpu_usage(pid_t, bool use_child_time = true) noexcept;

double cpu_usage_percent(bool use_child_time = true) noexcept;
double cpu_usage_percent(Process_Info&, bool use_child_time = true) noexcept;
double cpu_usage_percent(pid_t, bool use_child_time = true) noexcept;

/**
 * Usage during the amount of time determined
 */
unsigned long cpu_usage(pid_t, std::chrono::duration<int> const, bool use_child_time = true) noexcept;
unsigned long cpu_usage(std::chrono::duration<int> const, bool use_child_time = true) noexcept;

double cpu_usage_percent(pid_t, std::chrono::duration<int> const, bool use_child_time = true) noexcept;
double cpu_usage_percent(std::chrono::duration<int> const, bool use_child_time = true) noexcept;

double cpu_usage(Process_Info const& start, Process_Info const& end,
				bool use_child_time = true) noexcept;
double cpu_usage_percent(Process_Info const& start, Process_Info const& end,
				std::chrono::duration<int> const duration,
				bool use_child_time = true) noexcept;

double memory_usage_percent(Process_Info&, Hardware_Memory_Usage&) noexcept;
double memory_usage_percent(pid_t) noexcept;
double memory_usage_percent() noexcept;

bool memory_peak_usage(pid_t, Memory_Peak_Usage&) noexcept;
bool memory_peak_usage(Memory_Peak_Usage&) noexcept;

}//System

#endif /* SYSTEM_PROC_INFO_HPP__ */
