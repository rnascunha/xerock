#include "my_async.hpp"
#include <vector>
#include <string>
#include <iostream>

#include "my_async.hpp"

#include <memory>

using Serial = My_Async::Serial::Serial_SCb<std::string>;

void print_serial_port_list(std::vector<std::pair<std::string, bool>> const& v){
	for(auto p : v){
		std::cout << p.first << " " << (p.second ? "opened" : "closed") << "\n";
	}
}

int main(){
	auto p = My_Async::Serial::get_port_list();
	print_serial_port_list(p);

	auto conn = My_Async::Serial::scan_ports();
	if(conn.size() == 0){
		std::cout << "No serial ports to connect\n";
		return EXIT_SUCCESS;
	}
	auto port  = conn[0];

	boost::asio::io_context ioc;
	auto serial = std::make_shared<Serial>(ioc);
	try{
		serial->open(port.c_str(), 9600);
	} catch(My_Async::Serial::Serial_Exception& ex){
		std::cout << "Erro opening serial port " << port << "\n";
		return EXIT_FAILURE;
	}

	std::cout << "---------------\n";
	p = My_Async::Serial::get_port_list();
	print_serial_port_list(p);

	My_Async::Serial::add_port("/dev/ttyUSB1");
	My_Async::Serial::add_port("/dev/ttyUSB2");
	My_Async::Serial::add_port("/dev/ttyUSB3");

	std::cout << "---------------\n";
	p = My_Async::Serial::get_port_list();
	print_serial_port_list(p);

	return EXIT_SUCCESS;
}
