#include "proc_info.hpp"
#include "print_system.hpp"

#include <iostream>
#include <chrono>
#include <iomanip>

static void how_to_message(const char* name){
	std::cout << "Modo de usar:\n\t" << name << " <pid> [<interval(seconds)>]\n";
	std::cout << "Exemplo:\n\t" << name << " 5543 1 //Ler dados a cada 1s\n";
	std::cout << "\t" << name << " 5543 //Media dos dados ate o momento\n";
}

int
main(int argc, char* argv[])
{
	if(argc != 2 && argc != 3) {
		how_to_message(argv[0]);
		return EXIT_FAILURE;
	}
	try{
		int interval = 0;

		pid_t pid = std::stoi(argv[1]);
		if(argc == 3)
			interval = std::stoi(argv[2]);

		using namespace System;
		Process_Info info;
		Hardware_Memory_Usage mem_usage;

		if(!process_info(pid, info)){
			std::cout << "Error: process id [" << pid << "] doesn't exists\n";
			return EXIT_FAILURE;
		}

		std::cout << "pid: " << pid << " / interval: " << interval << "\n";

		if(!hardware_memory_usage(mem_usage)) return EXIT_FAILURE;

		print_process_info(info);
		print_hardware_memory_usage(mem_usage);
		print_all_cpu_usage(info);

		if(!interval){
			auto cpu_percent = cpu_usage_percent(info);
			auto mem_percent = memory_usage_percent(info, mem_usage);

			std::cout << "%CPU/%MEM = " << std::setprecision(1) << std::fixed <<
					cpu_percent << " / " << mem_percent << "\n";
		} else {
			while(true){
				auto cpu_percent = cpu_usage_percent(pid, std::chrono::seconds(interval));
				auto mem_percent = memory_usage_percent(pid);

				std::cout << "%CPU/%MEM = " << std::setprecision(1) << std::fixed <<
						cpu_percent << " / " << mem_percent << "\n";
			}
		}
	}catch(std::exception& ex){
		std::cout << "Error: " << argv[1] << " is not a valid number\n";
		how_to_message(argv[0]);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
