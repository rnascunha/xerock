#ifndef MONITOR_JSON_MAKE_MESSAGE_HPP__
#define MONITOR_JSON_MAKE_MESSAGE_HPP__

#include "core/message.hpp"
#include "system_types.hpp"

namespace Apps{
namespace Monitor{

Message::Type_Send make_monitor_message(double cpu_usage, double mem_usage_per,
										unsigned long cpu_usage_overall,
										double cpu_usage_overall_per,
										double uptime, int sample,
										unsigned num_threads, System::Memory_Peak_Usage& peak_mem,
										const System::Process_Info& start,
										const System::Process_Info& end);

}//Monitor
}//Apps

#endif /* MONITOR_JSON_MAKE_MESSAGE_HPP__ */
