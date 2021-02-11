#include "my_async.hpp"
#include <string>
#include <memory>

using udp_echo_client = My_Async::UDP::Client_Static_Callback<std::string>;

void on_open(std::shared_ptr<udp_echo_client> socket) noexcept
{
	std::cout << "Socket opened: "
		<< socket->local_endpoint().address().to_string() << ":"
		<< socket->local_endpoint().port() << " > "
		<< socket->endpoint().address().to_string() << ":"
		<< socket->endpoint().port() << "\n";
}

void on_close(std::shared_ptr<udp_echo_client>, boost::system::error_code) noexcept
{
	std::cout << "Socket closed\n";
}

void on_error(std::shared_ptr<udp_echo_client>, boost::system::error_code ec, char const* what) noexcept
{
	std::cerr << what << " [" << ec.value() << "] " << ec.message() << "\n";
}

void on_message(std::shared_ptr<udp_echo_client> socket, std::string data) noexcept
{
	std::cout << "Received: " << data << "\n";
	if(data == "quit\n")
	{
		boost::system::error_code ec;
		socket->close(ec);
		return;
	}

	socket->write(data);
}

int main(int argc, char** argv)
{
	/**
	 * Handling input
	 */
	if(argc != 3)
	{
		std::cerr << "Error! Use:\n\t" << argv[0] << " <addr> <port>\n";
		return EXIT_FAILURE;
	}

	boost::system::error_code ec;
	auto const address = boost::asio::ip::make_address(argv[1], ec);
	if(ec)
	{
		std::cerr << "ERROR! [" << argv[1] << "] Invalid address\n";
		return EXIT_FAILURE;
	}
	auto port = static_cast<unsigned short>(std::atoi(argv[2]));

	/**
	 * UDP Code
	 */
	boost::asio::io_context ioc;

	udp_echo_client::set_on_open(on_open);
	udp_echo_client::set_on_close(on_close);
	udp_echo_client::set_on_message(on_message);
	udp_echo_client::set_on_error(on_error);

	auto socket = std::make_shared<udp_echo_client>(ioc);

	socket->open(boost::asio::ip::udp::endpoint(address, port));
	socket->write(std::string("teste"));

	ioc.run();

	return EXIT_SUCCESS;
}
