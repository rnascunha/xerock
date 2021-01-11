#include "hardware_info.hpp"
#include "print_system.hpp"

#include <iostream>
#include <iomanip>

using namespace System;

template<typename type>
static void print_hardware_cpu_time(std::string name, CPU_Time<type> const& t) noexcept{
	std::cout << name << "\t"
			<< std::setprecision(2) << std::fixed
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

int main(){
	Hardware_CPU_Usage usage;
	if(!hardware_cpu_usage(usage)) return EXIT_FAILURE;
	print_hardware_cpu_usage(usage);

	CPU_Time<double> percent;
	hardware_cpu_time_percent(percent, usage.cores["cpu"]);
	std::cout << "Overall usage:\n";
	print_hardware_cpu_time("cpu", percent);

	while(true){
		hardware_cpu_time_percent(percent, std::chrono::seconds(3));
		print_hardware_cpu_time("cpu", percent);
	}

	return EXIT_SUCCESS;
}

