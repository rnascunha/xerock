#include <iostream>

#include "byte_array.h"
#include "my_async/my_async.hpp"

class Serial : public My_Async::Serial::Serial_Base<Byte_Array>{
	public:
		//Importing constructor from base class;
		using My_Async::Serial::Serial_Base<Byte_Array>::Serial_Base;

	protected:
		void fail(boost::system::error_code ec, char const* what) const noexcept{
			std::cerr << what << ": " << ec.message() << "\n";
		}

		virtual void read_time_handler(Byte_Array& data) noexcept{
			std::cout << "data: " << data.to_hex() << "\n";
		}
};

#define SERIAL_PORT_NAME	"/dev/ttyUSB0"
//#define USE_EXCEPTION

int main(){
	boost::asio::io_context ioc;

#ifndef USE_EXCEPTION
	boost::system::error_code ec;
	auto serial = std::make_shared<Serial>(ioc);

	serial->open(SERIAL_PORT_NAME, 9600, ec);
	if(ec){
		std::cerr << "Port not opened " SERIAL_PORT_NAME "\n";
		return EXIT_FAILURE;
	}
	std::cout << "Port opened " SERIAL_PORT_NAME "\n";


	//	Capture SIGINT and SIGTERM to perform a clean shutdown
	boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
	signals.async_wait(
		[&ioc](boost::system::error_code const&, int){
			ioc.stop();
		});

	Byte_Array b{0xC3,0xC3,0xC3};
	serial->write(b);

	boost::asio::steady_timer read_wait_(ioc);
	read_wait_.expires_after(std::chrono::seconds(1));

	read_wait_.async_wait([&serial](const boost::system::error_code& error __attribute__((unused))){
		serial->write({0xC1, 0xC1, 0xC1});
	});

	ioc.run();
#else
	try{
		auto serial = std::make_shared<Serial>(ioc);

		serial->open(SERIAL_PORT_NAME, 9600);
		std::cout << "Port opened " SERIAL_PORT_NAME "\n";

		//	Capture SIGINT and SIGTERM to perform a clean shutdown
		boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
		signals.async_wait(
			[&ioc](boost::system::error_code const&, int){
				ioc.stop();
			});

		Byte_Array b{0xC3,0xC3,0xC3};
		serial->write(b);

		boost::asio::steady_timer read_wait_(ioc);
		read_wait_.expires_after(std::chrono::seconds(1));

		read_wait_.async_wait([&serial](const boost::system::error_code& error __attribute__((unused))){
			serial->write({0xC1, 0xC1, 0xC1});
		});

		ioc.run();
	}catch(My_Async::Serial::Serial_Exception& ex){
		std::cerr << ex.error << ": " << ex.what() << "\n";
		std::cerr << "Port not opened " SERIAL_PORT_NAME "\n";

		return EXIT_FAILURE;
	}
#endif
	return EXIT_SUCCESS;
}
