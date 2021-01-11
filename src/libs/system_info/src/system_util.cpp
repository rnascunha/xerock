#include <unistd.h>
#include "system_util.hpp"

#define PAGE_SIZE_KB	(::sysconf(_SC_PAGESIZE) / 1024)
#define TICKS_PER_SEC	(::sysconf(_SC_CLK_TCK))

namespace System{

double ticks_to_seconds(unsigned long cpu_time){
	return static_cast<double>(cpu_time) / TICKS_PER_SEC;
}

double ticks_to_percent(unsigned long cpu_time, std::chrono::duration<int> const duration){
	return (100.0 * ticks_to_seconds(cpu_time)) /
			static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
}

unsigned long pages_to_kb(unsigned long mem_pages){
	return mem_pages * PAGE_SIZE_KB;
}

}
