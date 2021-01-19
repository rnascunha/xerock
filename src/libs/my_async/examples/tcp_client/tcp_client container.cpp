#include <boost/asio.hpp>

#include "my_async.hpp"
#include <memory>
#include "byte_array.h"

#include "load_certificate.hpp"
#include <iostream>

template<bool UseSSL>
using TCP_Echo_Client = typename My_Async::TCP::Client_Static_Callback<UseSSL, Byte_Array>;

template<bool UseSSL>
void read_handler(std::shared_ptr<TCP_Echo_Client<UseSSL>> socket, Byte_Array data) noexcept
{
	auto local = socket->local_endpoint();
	std::cout << "Received[" << local.address() << ":"
			<< local.port() << "]: " << data.to_string() << "\n";
	socket->write(data);
}

template<bool UseSSL>
void on_open(std::shared_ptr<TCP_Echo_Client<UseSSL>> socket) noexcept
{
	auto local = socket->local_endpoint();
	auto remote = socket->endpoint();
	std::cout << "Opened: " << local.address() << ":" << local.port() << " -> "
			<< remote.address() << ":" << remote.port() << "\n";
}

template<bool UseSSL>
void on_error(std::shared_ptr<TCP_Echo_Client<UseSSL>>,
		boost::system::error_code ec, char const* what) noexcept
{
	std::cout << "ERROR[" << what << "]: (" << ec.value() << ") " << ec.message() << "\n";
}

template<bool UseSSL>
void on_close(std::shared_ptr<TCP_Echo_Client<UseSSL>>,
		boost::system::error_code) noexcept
{
	std::cout << "Closed\n";
}

int
main(int argc, char* argv[])
{
	if(argc != 7)
	{
		std::cerr << "Error! Use:\n\t" << argv[0] << " <key> <cert> <addr> <port_plain> <port_ssl> <number>\n";
		return EXIT_FAILURE;
	}

	boost::system::error_code ec;
	auto const address = boost::asio::ip::make_address(argv[3], ec);
	if(ec)
	{
		std::cerr << "ERROR! [" << argv[3] << "] Invalid address\n";
		return EXIT_FAILURE;
	}
	auto port_plain = static_cast<unsigned short>(std::atoi(argv[4]));
	auto port_ssl = static_cast<unsigned short>(std::atoi(argv[5]));
	auto number = static_cast<unsigned short>(std::atoi(argv[6]));

//	 The io_context is required for all I/O
	boost::asio::io_context ioc;

	boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};

	load_certificate(ctx, argv[2], argv[1], ec);
	if(ec)
	{
		std::cerr << "ERROR! [" << ec.value() << "] " << ec.message() << "\n";
		return EXIT_FAILURE;
	}

	TCP_Echo_Client<true>::set_on_open(on_open<true>);
	TCP_Echo_Client<true>::set_on_close(on_close<true>);
	TCP_Echo_Client<true>::set_on_message(read_handler<true>);
	TCP_Echo_Client<true>::set_on_error(on_error<true>);

	TCP_Echo_Client<false>::set_on_open(on_open<false>);
	TCP_Echo_Client<false>::set_on_close(on_close<false>);
	TCP_Echo_Client<false>::set_on_message(read_handler<false>);
	TCP_Echo_Client<false>::set_on_error(on_error<false>);

	My_Async::TCP::Container<true, true> container(ioc, ctx);

	std::cout << "Initiating " << number << " sockets\n";
	for(auto i = 1; i <= number; i++)
	{

		boost::system::error_code ec;
		if(i % 2)
			container.open<TCP_Echo_Client<false>>(
				boost::asio::ip::tcp::endpoint{address, port_plain},
				ec);
		else
			container.open<TCP_Echo_Client<true>>(
							boost::asio::ip::tcp::endpoint{address, port_ssl},
							ec);

		if(ec) std::cout << "[" << ec.value() << "] " << ec.message() << "\n";
	}

	// Capture SIGINT and SIGTERM to perform a clean shutdown
	boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
	signals.async_wait(
		[&ioc](boost::system::error_code const&, int){
			ioc.stop();
		});

	ioc.run();

	return EXIT_SUCCESS;
}
