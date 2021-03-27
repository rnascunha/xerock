#include "my_async.hpp"
#include <string>
#include <memory>

template<typename derived>
using base_class = My_Async::UDP::Client_Base<derived, std::string>;

class udp_echo_client : public base_class<udp_echo_client>,
					public std::enable_shared_from_this<udp_echo_client>{
	public:
		//Importing constructors from base class
		using base_class<udp_echo_client>::base_class;

	protected:
		void on_open() noexcept
		{
			std::cout << "Socket opened: "
				<< local_endpoint().address().to_string() << ":"
				<< local_endpoint().port() << " > "
				<< endpoint().address().to_string() << ":"
				<< endpoint().port() << "\n";
		}

		void on_close(boost::system::error_code) noexcept
		{
			std::cout << "Socket closed\n";
		}

		void on_error(boost::system::error_code ec, char const* what) noexcept
		{
			std::cerr << what << " [" << ec.value() << "] " << ec.message() << "\n";
		}

		void read_handler(std::string data) noexcept
		{
			std::cout << "Received: " << data << "\n";
			if(data == "quit\n") {
				boost::system::error_code ec;
				close(ec);
				return;
			}

			write(data);
		}
};

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

	auto socket = std::make_shared<udp_echo_client>(ioc);

	socket->open(boost::asio::ip::udp::endpoint(address, port));
	socket->write(std::string("teste"));

	ioc.run();

	return EXIT_SUCCESS;
}
