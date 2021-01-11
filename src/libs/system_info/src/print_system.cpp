#include "print_system.hpp"

#include <iostream>

#include "proc_info.hpp"
#include <iomanip>

namespace System{

static void print_memory_usage(const Memory_Usage& usage){
	std::cout << "\ttotal:\t" << usage.total << "\n"
			<< "\trss:\t" << usage.rss << "\n"
			<< "\tshared:\t" << usage.shared << "\n"
			<< "\ttext:\t" << usage.text << "\n"
			<< "\tdata:\t" << usage.data << "\n";
}

static void print_cpu_usage(const CPU_Usage& usage){
	std::cout << "\tutime:\t" << usage.utime << "\n"
			<< "\tstime:\t" << usage.stime << "\n"
			<< "\tcutime:\t" << usage.cutime << "\n"
			<< "\tcstime:\t" << usage.cstime << "\n"
			<< "\tstarttime:\t" << usage.starttime << "\n";
}

static void print_cpu_usage_header(){
	std::cout << "name\tut\tst\tcut\tcst\n";
}

static void print_cpu_usage_short(const char* name, const CPU_Usage& usage){
	std::cout << name << "\t"
			<< usage.utime << "\t"
			<< usage.stime << "\t"
			<< usage.cutime << "\t"
			<< usage.cstime << "\t"
			<< usage.starttime << "\n";
}

void print_all_cpu_usage(Process_Info& info){
	print_cpu_usage_header();
	print_cpu_usage_short("main", info.cusage);
	int i = 0;
	for(auto p : info.tpid){
		print_cpu_usage_short(std::to_string(p).c_str(), info.tcusage[i++]);
	}
}

void print_all_cpu_usage(pid_t pid)
{
	Process_Info info;
	if(!process_info(pid, info)) return;

	print_all_cpu_usage(info);
}

void print_all_cpu_usage()
{
	Process_Info info;
	if(!process_info(info)) return;

	print_all_cpu_usage(info);
}

void print_process_info(Process_Info& info){
	std::cout << "PID/P:\t\t" << info.pid << " / " << info.ppid << "\n";
	std::cout << "Command:\t";
	for(auto const& a : info.cmd){
		std::cout << a << " ";
	}
	std::cout << "\nMemory usage:\n";
	print_memory_usage(info.usage);
	std::cout << "\nCPU usage:\n";
	print_cpu_usage(info.cusage);

	std::cout << "\nThreads:\t";
	for(auto const p : info.tpid){
		std::cout << p << " ";
	}

	std::cout << "\n";
}

void print_process_info(pid_t pid){
	Process_Info info;
	if(!process_info(pid, info)){
		std::cout << "Error getting process info\n";
		return;
	}

	print_process_info(info);
}

void print_process_info(){
	Process_Info info;
	if(!process_info(info)){
		std::cout << "Error getting process info\n";
		return;
	}

	print_process_info(info);
}

void print_hardware_memory_usage(Hardware_Memory_Usage& usage){
	std::cout << "MemTotal:\t" << usage.total << "\tkB\n"
			<< "MemFree:\t" <<  usage.free << "\tkB\n"
			<< "MemAvailable:\t" << usage.available << "\tkB\n"
			<< "Buffers:\t" << usage.buffer << "\tkB\n"
			<< "Cached:\t\t" << usage.cached << "\tkB\n"
			<< "SwapCached:\t" << usage.swap_cached << "\tkB\n"
			<< "SwapTotal:\t" << usage.swap_total << "\tkB\n"
			<< "SwapFree:\t" << usage.swap_free << "\tkB\n";
}

template<typename type>
static void print_hardware_cpu_time(std::string name, CPU_Time<type> const& t) noexcept{
	std::cout << name << "\t"
			<< t.user << "\t"
			<< t.nice << "\t"
			<< t.system << "\t"
			<< t.idle << "\t"
			<< t.iowait << "\t"
			<< t.irq << "\t"
			<< t.soft_irq << "\t"
			<< t.steal << "\t"
			<< t.guest << "\t"
			<< t.guest_nice << "\n";
}

void print_hardware_cpu_usage(Hardware_CPU_Usage const& usage) noexcept{
	std::cout << "Process/running/blocked: "
			<< usage.processes << " / "
			<< usage.process_running << " / "
			<< usage.process_blocked << "\n";
	//Header
	std::cout << "name\tuser\tnice\tsystem\tidle\t\tiowait\tirq\tsoftirq\tsteal\tguest\tguest_nice\n";
	for(auto const& [name, t] : usage.cores){
		print_hardware_cpu_time(name, t);
	}
}

}//System
