#include "../make.hpp"
#include "system_util.hpp"

#include "rapidjson/document.h"
#include "defines.hpp"

namespace Apps{
namespace Monitor{

Message::Type_Send make_monitor_message(double cpu_time, double mem_usage_per,
										unsigned long cpu_usage_overall,
										double cpu_usage_overall_per,
										double uptime, int sample,
										unsigned num_threads, System::Memory_Peak_Usage& peak_mem,
										const System::Process_Info& start,
										const System::Process_Info&)
{
	rapidjson::Document d;
	d.SetObject();
	d.AddMember(MONITOR_APP_CPU_TIME, System::ticks_to_seconds(cpu_time), d.GetAllocator());
	d.AddMember(MONITOR_APP_CPU_PERCENT,
			System::ticks_to_percent(cpu_time, std::chrono::seconds(sample)), d.GetAllocator());
	d.AddMember(MONITOR_APP_CPU_OVERALL, System::ticks_to_seconds(cpu_usage_overall), d.GetAllocator());
	d.AddMember(MONITOR_APP_CPU_OVERALL_PERCENT, cpu_usage_overall_per, d.GetAllocator());
	d.AddMember(MONITOR_APP_MEM_KB, System::pages_to_kb(start.usage.rss), d.GetAllocator());
	d.AddMember(MONITOR_APP_MEM_VM_KB, System::pages_to_kb(start.usage.total), d.GetAllocator());
	d.AddMember(MONITOR_APP_MEM_PERCENT, mem_usage_per, d.GetAllocator());
	d.AddMember(MONITOR_APP_UPTIME, uptime, d.GetAllocator());
	d.AddMember(MONITOR_APP_NUM_THREADS, num_threads, d.GetAllocator());
	d.AddMember(MONITOR_APP_MEM_PEAK_KB, peak_mem.mem_peak, d.GetAllocator());
	d.AddMember(MONITOR_APP_MEM_VM_PEAK_KB, peak_mem.virtual_mem_peak, d.GetAllocator());
	d.AddMember(MONITOR_APP_SAMPLE_TIME, sample, d.GetAllocator());

	return Message::Factory::create(Message::App::monitor, d);
}

}//Monitor
}//Apps
