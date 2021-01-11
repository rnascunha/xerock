#include <iostream>

#include "byte_array.h"
#include "my_async/my_async.hpp"

#define SERIAL_PORT_NAME	"/dev/ttyUSB0"
//#define USE_EXCEPTION

using Serial = My_Async::Serial::Serial_Callback<Byte_Array>;

static void on_open_serial(){
	std::cout << "Opened serial port" << "\n";
}

static void on_message_serial(Byte_Array const& data){
	std::cout << "data: " << data.to_hex() << "\n";
}

static void on_error_serial(boost::system::error_code ec, char const* what){
	std::cerr << what << ": " << ec.message() << "\n";
}

static void on_close_serial(){
	std::cout << "Closed serial port\n";
}

int main(){
	boost::asio::io_context ioc;

#ifndef USE_EXCEPTION
	boost::system::error_code ec;
	auto serial = std::make_shared<Serial>(ioc);

	serial->on_open(on_open_serial);
	serial->on_message(on_message_serial);
	serial->on_error(on_error_serial);
	serial->on_close(on_close_serial);

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

	boost::asio::steady_timer read_wait_(ioc),
								close_port(ioc);

	read_wait_.expires_after(std::chrono::seconds(1));
	close_port.expires_after(std::chrono::seconds(2));

	read_wait_.async_wait([&serial](const boost::system::error_code&){
		serial->write({0xC1, 0xC1, 0xC1});
	});

	close_port.async_wait([serial, &signals
						   ](const boost::system::error_code&){
		boost::system::error_code ec;
		serial->close(ec);
		signals.cancel(ec);
	});

	ioc.run();
#else
	try{
		auto serial = std::make_shared<Serial>(ioc);

		serial->on_open(on_open_serial);
		serial->on_message(on_message_serial);
		serial->on_error(on_error_serial);
		serial->on_close(on_close_serial);

		serial->open(SERIAL_PORT_NAME, 9600);

		//	Capture SIGINT and SIGTERM to perform a clean shutdown
		boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
		signals.async_wait(
			[&ioc](boost::system::error_code const&, int){
				ioc.stop();
			});

		Byte_Array b{0xC3,0xC3,0xC3};
		serial->write(b);

		boost::asio::steady_timer read_wait_(ioc),
									close_port(ioc);

		read_wait_.expires_after(std::chrono::seconds(1));
		close_port.expires_after(std::chrono::seconds(2));

		read_wait_.async_wait([&serial](const boost::system::error_code& error __attribute__((unused))){
		serial->write({0xC1, 0xC1, 0xC1});
		});

		close_port.async_wait([serial, &signals](const boost::system::error_code& error __attribute__((unused))){
			serial->close();
			signals.cancel();
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
