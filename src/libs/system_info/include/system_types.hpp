#ifndef SYSTEM_TYPES_HPP__
#define SYSTEM_TYPES_HPP__

#include <vector>
#include <map>
#include <string>

namespace System{

struct Memory_Usage{
	unsigned long total;
	long 		  rss;		//Resident set size (RAM)
	unsigned long shared;
	unsigned long text;		//Code
	unsigned long data;		//data + stack
};

// /proc/meminfo
struct Hardware_Memory_Usage{
	unsigned long total;
	unsigned long free;
	unsigned long available;
	unsigned long buffer;
	unsigned long cached;
	unsigned long swap_cached;
	unsigned long swap_total;
	unsigned long swap_free;
};

struct CPU_Usage{
	unsigned long utime;
	unsigned long stime;
	unsigned long cutime;
	unsigned long cstime;
	unsigned long long starttime;
};

// /proc/stat
template<typename type>
struct CPU_Time{
	type user;
	type nice;
	type system;
	type idle;
	type iowait;
	type irq;
	type soft_irq;
	type steal;
	type guest;
	type guest_nice;
};

struct Hardware_CPU_Usage{
	std::map<std::string, CPU_Time<unsigned long>> cores;
	unsigned long processes;
	unsigned long process_running;
	unsigned long process_blocked;
};

struct Uptime{
	double total;
	double idle;
};

struct Process_Info{
	pid_t pid;
	pid_t ppid;
	Memory_Usage usage;
	CPU_Usage cusage;
	std::vector<std::string> cmd;
	std::vector<pid_t> tpid;
	std::vector<CPU_Usage> tcusage;

	void clear(){
		cmd.clear();
		tpid.clear();
		tcusage.clear();
	}
};

struct Memory_Peak_Usage{
	unsigned long mem_peak;
	unsigned long virtual_mem_peak;
};

}//System

#endif /* SYSTEM_TYPES_HPP__ */
