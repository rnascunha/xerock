#include "proc_info.hpp"

#include <fstream>

#include <cmath>

#include <filesystem>
#include <limits>       // std::numeric_limits

#include <thread>
#include <unistd.h>

#include "hardware_info.hpp"

/*
 * http://man7.org/linux/man-pages/man5/proc.5.html
 */

namespace System{

//Files
static constexpr const char* proc = "/proc";
static constexpr const char* cmdline = "cmdline";
static constexpr const char* task = "task";
static constexpr const char* statm = "statm";
static constexpr const char* stat = "stat";
static constexpr const char* uptime_ = "uptime";
static constexpr const char* status = "status";

#define PAGE_SIZE_KB	(::sysconf(_SC_PAGESIZE) / 1024)
#define TICKS_PER_SEC	(::sysconf(_SC_CLK_TCK))

static bool memory_usage(const std::string& base, Memory_Usage& usage) noexcept;
static bool cpu_usage(const std::string& base, CPU_Usage& usage) noexcept;
static bool get_process_info(const std::string& base, Process_Info& info) noexcept;

bool process_info(Process_Info& info) noexcept{
	info.pid = ::getpid();

	return get_process_info(std::string(proc) + "/" + std::to_string(info.pid), info);
}

bool process_info(pid_t pid, Process_Info& info) noexcept{
	std::string path(std::string(proc) + "/" + std::to_string(pid));

	if(!std::filesystem::exists(path)) return false;

	return get_process_info(path, info);
}

bool uptime(Uptime& up) noexcept{
	std::ifstream in(std::string(proc) + "/" + uptime_);
	if(!in.is_open()) return false;

	in >> up.total;
	in >> up.idle;

	in.close();

	return true;
}

bool process_uptime(Process_Info& info, double& up) noexcept{
	Uptime up_;
	if(!uptime(up_)) return false;

	up = up_.total - info.cusage.starttime / TICKS_PER_SEC;

	return true;
}

bool process_uptime(pid_t pid, double& uptime) noexcept{
	Process_Info info;
	info.pid = pid;
	if(!process_info(info)) return false;

	return process_uptime(info, uptime);
}

bool process_uptime(double& uptime) noexcept{
	return process_uptime(::getpid(), uptime);
}

unsigned long cpu_usage(pid_t pid, std::chrono::duration<int> const duration, bool use_child_time /* = true */) noexcept{
	if(duration.count() == 0) return -1;

	Process_Info start, end;
	if(!process_info(pid, start)) return -1;
	std::this_thread::sleep_for(duration);
	if(!process_info(pid, end)) return -1;

	auto total_time = (end.cusage.utime - start.cusage.utime) +
			(end.cusage.stime - start.cusage.stime);
	if(use_child_time){
		total_time += (end.cusage.cutime - start.cusage.cutime) +
				(end.cusage.cstime - start.cusage.cstime);
	}

	return total_time;
}

unsigned long cpu_usage(std::chrono::duration<int> const duration, bool use_child_time /* = true */) noexcept{
	return cpu_usage(::getpid(), duration, use_child_time);
}

unsigned long cpu_usage(Process_Info& info, bool use_child_time /* = true */) noexcept
{
	auto total_time = info.cusage.utime + info.cusage.stime;
	if(use_child_time){
		total_time += info.cusage.cutime + info.cusage.cstime;
	}

	return  total_time;
}

unsigned long cpu_usage(pid_t pid, bool use_child_time /* = true */) noexcept
{
	Process_Info info;
	info.pid = pid;
	if(!process_info(info)) return -1;

	return cpu_usage(info, use_child_time);
}

unsigned long cpu_usage(bool use_child_time /* = true */) noexcept
{
	return cpu_usage(::getpid(), use_child_time);
}

double cpu_usage_percent(Process_Info& info, bool use_child_time /* = true */) noexcept{
	Uptime up;
	if(!uptime(up)) return -1;

	auto total_time = info.cusage.utime + info.cusage.stime;
	if(use_child_time){
		total_time += info.cusage.cutime + info.cusage.cstime;
	}

	auto seconds = std::lround(up.total) - (info.cusage.starttime / TICKS_PER_SEC);

	if(seconds)
		return  ((100 *(static_cast<double>(total_time) / TICKS_PER_SEC)) / seconds);
	return 0;
}

double cpu_usage_percent(pid_t pid, bool use_child_time /* = true */) noexcept{
	Process_Info info;
	if(!process_info(pid, info)) return -1;

	return cpu_usage_percent(info, use_child_time);
}

double cpu_usage_percent(bool use_child_time /* = true */) noexcept{
	Process_Info info;
	if(!process_info(info)) return -1;

	return cpu_usage_percent(info, use_child_time);
}

double cpu_usage_percent(std::chrono::duration<int> const duration, bool use_child_time /* = true */) noexcept{
	return cpu_usage_percent(::getpid(), duration, use_child_time);
}

double cpu_usage_percent(pid_t pid, std::chrono::duration<int> const duration, bool use_child_time /* = true */) noexcept{
	if(duration.count() == 0) return -1;

	Process_Info start, end;
	if(!process_info(pid, start)) return -1;
	std::this_thread::sleep_for(duration);
	if(!process_info(pid, end)) return -1;

	auto total_time = (end.cusage.utime - start.cusage.utime) +
			(end.cusage.stime - start.cusage.stime);
	if(use_child_time){
		total_time += (end.cusage.cutime - start.cusage.cutime) +
				(end.cusage.cstime - start.cusage.cstime);
	}

	return (100.0 * static_cast<double>(total_time) /
				static_cast<double>(TICKS_PER_SEC)) /
				static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
}

double cpu_usage(
		Process_Info const& start, Process_Info const& end,
		bool use_child_time /* = true */) noexcept
{
	auto total_time = (end.cusage.utime - start.cusage.utime) +
				(end.cusage.stime - start.cusage.stime);
	if(use_child_time){
		total_time += (end.cusage.cutime - start.cusage.cutime) +
				(end.cusage.cstime - start.cusage.cstime);
	}

	return total_time;
}

double cpu_usage_percent(Process_Info const& start, Process_Info const& end,
		std::chrono::duration<int> const duration,
		bool use_child_time /* = true */) noexcept
{
	if(duration.count() == 0) return -1;

	auto total_time = cpu_usage(start, end, use_child_time);

	return (100.0 * static_cast<double>(total_time) /
			static_cast<double>(TICKS_PER_SEC)) /
			static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
}

double memory_usage_percent(Process_Info& info, Hardware_Memory_Usage& mem_usage) noexcept
{
	return (static_cast<double>(100 * info.usage.rss * PAGE_SIZE_KB) / (mem_usage.total));
}

double memory_usage_percent(pid_t pid) noexcept{
	Process_Info info;
	if(!process_info(pid, info)) return -1;

	Hardware_Memory_Usage mem_usage;
	if(!hardware_memory_usage(mem_usage)) return -1;

	return memory_usage_percent(info, mem_usage);
}

double memory_usage_percent() noexcept{
	return memory_usage_percent(::getpid());
}

bool memory_peak_usage(pid_t pid, Memory_Peak_Usage& peak) noexcept
{
	std::ifstream in(std::string(proc) + "/" + std::to_string(pid) + "/" + status);
	if(!in.is_open()) return false;

	bool  flag_vm = true, flag_rss = true;
	do{
		std::string input;
		in >> input;
		if(input == std::string("VmHWM:"))
		{
			in >> peak.mem_peak;
			flag_rss = false;
		}

		if(input == std::string("VmPeak:"))
		{
			in >> peak.virtual_mem_peak;
			flag_vm = false;
		}
		in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}while(in.eofbit && (flag_vm || flag_rss));

	return true;
}

bool memory_peak_usage(Memory_Peak_Usage& peak) noexcept
{
	return memory_peak_usage(::getpid(), peak);
}

/**
 *
 */

static bool memory_usage(const std::string& base, Memory_Usage& usage) noexcept{
	std::ifstream in(base + "/" + statm);
	if(!in.is_open()) return false;

	std::string input;
	in >> input;
	usage.total = std::atol(input.c_str());
	in >> input;
	usage.rss = std::atol(input.c_str());
	in >> input;
	usage.shared = std::atol(input.c_str());
	in >> input;
	usage.text = std::atol(input.c_str());
	in >> input;
	in >> input;
	usage.data = std::atol(input.c_str());

	in.close();

	return true;
}

static bool cpu_usage(const std::string& base, CPU_Usage& usage) noexcept{
	std::ifstream in(base + "/" + stat);
	if(!in.is_open()) return false;

	int i = 13;
	while(i--) in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');

	std::string input;
	in >> input;
	usage.utime = std::atol(input.c_str());
	in >> input;
	usage.stime = std::atol(input.c_str());
	in >> input;
	usage.cutime = std::atol(input.c_str());
	in >> input;
	usage.cstime = std::atol(input.c_str());

	i = 5;
	while(i--) in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
	in >> input;
	usage.starttime = std::atoll(input.c_str());

	in.close();

	return true;
}

static bool get_process_info(const std::string& base, Process_Info& info) noexcept{
	{
		std::ifstream in(base + "/" + cmdline, std::ios::binary);
		if(!in.is_open()) return false;
		char c;
		std::string arg;
		while(in >> c){
			if(c == '\0'){
				info.cmd.push_back(arg);
				arg.clear();
			}
			else
				arg.push_back(c);
		}
		in.close();
	}
	{
		std::ifstream in(base + "/" + stat);
		if(!in.is_open()) return false;

		int i = 3;
		while(i--) in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
		in >> info.ppid;
		in.close();
	}
	{
		std::string path(base + "/" + task);
		for (const auto & entry : std::filesystem::directory_iterator(path))
			info.tpid.push_back(std::atoi(entry.path().filename().string().c_str()));
	}
	memory_usage(base, info.usage);
	cpu_usage(base, info.cusage);
	{
		for(auto p : info.tpid){
			CPU_Usage usage;
			cpu_usage(base + "/" + task + "/" + std::to_string(p), usage);
			info.tcusage.push_back(usage);
		}
	}

	return true;
}

}//System

