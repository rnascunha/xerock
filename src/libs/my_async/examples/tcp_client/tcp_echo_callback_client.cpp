#include <boost/asio.hpp>

#include "my_async.hpp"
#include <memory>
#include <string>

#if USE_SSL == 1
#include "load_certificate.hpp"
#endif /* USE_SSL == 1 */

#include <iostream>

template<bool UseSSL>
using TCP_Echo_Client = typename My_Async::TCP::Client_Callback<UseSSL, std::string>;

template<bool UseSSL>
void read_handler(std::shared_ptr<TCP_Echo_Client<UseSSL>> socket, std::string data) noexcept
{
	std::cout << "Received: " << data << "\n";
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
#if USE_SSL == 1
#define ADDR_INDEX	3
#define PORT_INDEX	4
	if(argc != 5)
	{
		std::cerr << "Error! Use:\n\t" << argv[0] << " <key> <cert> <addr> <port>\n";
		return EXIT_FAILURE;
	}
#else
#define ADDR_INDEX	1
#define PORT_INDEX	2
	if(argc != 3)
	{
		std::cerr << "Error! Use:\n\t" << argv[0] << " <addr> <port>\n";
		return EXIT_FAILURE;
	}
#endif
	boost::system::error_code ec;
	auto const address = boost::asio::ip::make_address(argv[ADDR_INDEX], ec);
	if(ec)
	{
		std::cerr << "ERROR! [" << argv[ADDR_INDEX] << "] Invalid address\n";
		return EXIT_FAILURE;
	}
	auto port = static_cast<unsigned short>(std::atoi(argv[PORT_INDEX]));

	// The io_context is required for all I/O
	boost::asio::io_context ioc;

#if USE_SSL == 1
	boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};

	load_certificate(ctx, argv[2], argv[1], ec);
	if(ec)
	{
		std::cerr << "ERROR! Loading SSL parameters\n";
		return EXIT_FAILURE;
	}

	auto client = std::make_shared<TCP_Echo_Client<true>>(ioc, ctx);

	client->set_on_open(on_open<true>);
	client->set_on_close(on_close<true>);
	client->set_on_message(read_handler<true>);
	client->set_on_error(on_error<true>);
#else
	auto client = std::make_shared<TCP_Echo_Client<false>>(ioc);

	client->set_on_open(on_open<false>);
	client->set_on_close(on_close<false>);
	client->set_on_message(read_handler<false>);
	client->set_on_error(on_error<false>);
#endif /* USE_SSL == 1 */
	client->open(boost::asio::ip::tcp::endpoint{address, port});

	// Capture SIGINT and SIGTERM to perform a clean shutdown
	boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
	signals.async_wait(
		[&ioc](boost::system::error_code const&, int){
			ioc.stop();
		});

	ioc.run();

	return EXIT_SUCCESS;
}
