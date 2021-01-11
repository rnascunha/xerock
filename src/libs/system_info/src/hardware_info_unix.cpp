#include "hardware_info.hpp"

#include <fstream>
#include <limits>       // std::numeric_limits

#include <chrono>

#include <thread>

static constexpr const char* proc = "/proc";
static constexpr const char* meminfo = "meminfo";
static constexpr const char* stat = "stat";

#define CONVERT_PERCENT(field)		percent.field = 100.0 * (static_cast<double>(time.field) / total_time)
#define DIFF_FIELD(field)			diff.field = end.field - start.field

namespace System{

static void get_cpu_time(std::ifstream& in, CPU_Time<unsigned long>& time);
static bool check_get_cpu_time(std::string const& cpu_name, CPU_Time<unsigned long>& time);
static void read_cpu_time(std::ifstream& in, Hardware_CPU_Usage& usage);
static unsigned long total_cpu_time(CPU_Time<unsigned long> const& time);

bool hardware_cpu_usage(Hardware_CPU_Usage& usage) noexcept{
	std::ifstream in(std::string(proc) + "/" + stat);
	if(!in.is_open()) return false;

	//Main cpu
	read_cpu_time(in, usage);
	{
		//Ignoring lines
		int i = 3;
		while(i--) 	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	//Process
	in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
	in >> usage.processes;
	//Process running
	in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
	in >> usage.process_running;
	//Process blocked
	in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
	in >> usage.process_blocked;

	in.close();

	return true;
}

void hardware_cpu_time_percent(CPU_Time<double>& percent, CPU_Time<unsigned long> const& time) noexcept{
	auto total_time = total_cpu_time(time);

	if(total_time == 0) return;

	CONVERT_PERCENT(user);
	CONVERT_PERCENT(nice);
	CONVERT_PERCENT(system);
	CONVERT_PERCENT(idle);
	CONVERT_PERCENT(iowait);
	CONVERT_PERCENT(irq);
	CONVERT_PERCENT(soft_irq);
	CONVERT_PERCENT(steal);
	CONVERT_PERCENT(guest);
	CONVERT_PERCENT(guest_nice);
}

bool hardware_cpu_time_percent(CPU_Time<double>& percent, std::string cpu /* = "cpu" */) noexcept{
	CPU_Time<unsigned long> time;
	if(!check_get_cpu_time(cpu, time)) return false;

	hardware_cpu_time_percent(percent, time);

	return true;
}

/**
 * @param cpu = "cpu", "cpu0", "cpu1",...
 */
bool hardware_cpu_time_percent(CPU_Time<double>& percent,
		std::chrono::duration<int> duration,
		std::string cpu /* = "cpu" */) noexcept
{
	CPU_Time<unsigned long> start, end;

	if(!check_get_cpu_time(cpu, start)) return false;
	std::this_thread::sleep_for(duration);
	if(!check_get_cpu_time(cpu, end)) return false;

	CPU_Time<unsigned long> diff;
	DIFF_FIELD(user);
	DIFF_FIELD(nice);
	DIFF_FIELD(system);
	DIFF_FIELD(idle);
	DIFF_FIELD(iowait);
	DIFF_FIELD(irq);
	DIFF_FIELD(soft_irq);
	DIFF_FIELD(steal);
	DIFF_FIELD(guest);
	DIFF_FIELD(guest_nice);
	hardware_cpu_time_percent(percent, diff);

	return true;
}

bool hardware_memory_usage(Hardware_Memory_Usage& usage) noexcept{
	std::ifstream in(std::string(proc) + "/" + meminfo);
	if(!in.is_open()) return false;

	//Total line
	in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
	in >> usage.total;
	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	//Free line
	in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
	in >> usage.free;
	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	//Avaiable line
	in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
	in >> usage.available;
	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	//Buffer line
	in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
	in >> usage.buffer;
	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	//Cached
	//Avaiable line
	in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
	in >> usage.cached;
	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	//Swap Cached
	in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
	in >> usage.swap_cached;
	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	{
		//Ignoring lines
		int i = 8;
		while(i--) 	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	//Swap Total
	in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
	in >> usage.swap_total;
	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	//Swap Free
	in.ignore(std::numeric_limits<std::streamsize>::max(), ' ');
	in >> usage.swap_free;
	in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	in.close();

	return true;
}

/**
 *
 *
 *
 */
static unsigned long total_cpu_time(CPU_Time<unsigned long> const& time){
	return time.user +
			time.nice +
			time.system +
			time.idle +
			time.iowait +
			time.irq +
			time.soft_irq +
			time.steal +
			time.guest +
			time.guest_nice;
}

static void get_cpu_time(std::ifstream& in, CPU_Time<unsigned long>& time){
	in >> time.user;
	in >> time.nice;
	in >> time.system;
	in >> time.idle;
	in >> time.iowait;
	in >> time.irq;
	in >> time.soft_irq;
	in >> time.steal;
	in >> time.guest;
	in >> time.guest_nice;
}

static bool check_get_cpu_time(std::string const& cpu_name, CPU_Time<unsigned long>& time){
	std::ifstream in(std::string(proc) + "/" + stat);
	if(!in.is_open()) return false;

	while(true){
		std::string name;
		in >> name;
		if(name.find("cpu") != 0) return false;
		if(name == cpu_name){
			get_cpu_time(in, time);
			return true;
		} else {
			in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}
	}
}

static void read_cpu_time(std::ifstream& in, Hardware_CPU_Usage& usage){
	std::string name;
	CPU_Time<unsigned long> time;

	while(true){
		in >> name;
		if(name.find("cpu") != 0) return;
		get_cpu_time(in, time);
		usage.cores[name] = time;
	}
}

}//System
