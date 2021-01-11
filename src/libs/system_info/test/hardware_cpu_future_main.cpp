#include "hardware_info.hpp"
#include "print_system.hpp"

#include <iostream>
#include <iomanip>
#include <future>

//#define OTHER_WAY

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
	std::cout << "name\tuser\tnice\tsystem\tidle\tiowait\tirq\tsoftirq\tsteal\tguest\tguest_nice\n";
	while(true){
#ifndef OTHER_WAY
		//Captuing value at lambda
		CPU_Time<double> percent;
		std::future<void> fut = std::async(std::launch::async, [&]{
				hardware_cpu_time_percent(percent, std::chrono::seconds(3));
		});

		while (fut.wait_for(std::chrono::seconds(1)) == std::future_status::timeout)
		    std::cout << '.' << std::flush; //Work been done asyncronous

		std::cout << "\n";
		print_hardware_cpu_time("cpu", percent);
#else /* OTHER_WAY */
		//Returning value from lambda
		std::future<CPU_Time<double>> fut = std::async(std::launch::async, []{
				CPU_Time<double> percent;
				hardware_cpu_time_percent(percent, std::chrono::seconds(3));
				return percent;
		});

		while (fut.wait_for(std::chrono::seconds(1)) == std::future_status::timeout)
		    std::cout << '.' << std::flush; //Work been done asyncronous

		std::cout << "\n";
		print_hardware_cpu_time("cpu", fut.get());
#endif /* OTHER_WAY */
	}

	return EXIT_SUCCESS;
}
