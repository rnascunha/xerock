#ifndef SYSTEM_PRINT_HPP__
#define SYSTEM_PRINT_HPP__

#include "proc_info.hpp"

namespace System{

void print_process_info(Process_Info& info);
void print_process_info(pid_t pid);
void print_process_info();

void print_all_cpu_usage(Process_Info&);
void print_all_cpu_usage(pid_t);
void print_all_cpu_usage();

void print_hardware_memory_usage(Hardware_Memory_Usage&);
void print_hardware_cpu_usage(Hardware_CPU_Usage const&) noexcept;

}//System

#endif /* SYSTEM_PRINT_HPP__ */
