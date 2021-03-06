#include "my_async.hpp"
#include <string>
#include <memory>

template<typename derived>
using base_class = My_Async::UDP::Server_Base<derived, std::string>;

class udp_echo_server : public base_class<udp_echo_server>,
					public std::enable_shared_from_this<udp_echo_server>{
	public:
		//Importing constructors from base class
		using base_class<udp_echo_server>::base_class;

	protected:
		void on_open() noexcept
		{
			std::cout << "Socket opened: "
				<< local_endpoint().address().to_string() << ":"
				<< local_endpoint().port() << "\n";

			local_ = local_endpoint();
		}

		void on_close(boost::system::error_code) noexcept
		{
			std::cout << "Socket closed: "
					<< local_.address().to_string() << ":"
					<< local_.port() << "\n";
		}

		void on_error(boost::system::error_code ec, char const* what) noexcept
		{
			std::cerr << what << " [" << ec.value() << "] " << ec.message() << "\n";
		}

		void read_handler(std::string data) noexcept
		{
			std::cout << "Received["
					<< local_endpoint().address().to_string() << ":"
					<< local_endpoint().port() << "<->"
					<< endpoint().address().to_string() << ":"
					<< endpoint().port() << "]: " << data << "\n";
			if(data == "quit\n")
			{
				boost::system::error_code ec;
				close(ec);
				return;
			}

			write(endpoint(), data);
		}

		boost::asio::ip::udp::endpoint local_;
};

int main(int argc, char** argv)
{
	boost::asio::ip::udp::endpoint ep1, ep2;
	boost::system::error_code ec;

	/**
	 * Handling input
	 */
	if(argc == 2)
	{
		auto port1 = static_cast<unsigned short>(std::atoi(argv[1]));
		auto port2 = static_cast<unsigned short>(port1 + 1);
		ep1 = boost::asio::ip::udp::endpoint{boost::asio::ip::udp::v4(), port1};
		ep2 = boost::asio::ip::udp::endpoint{boost::asio::ip::udp::v4(), port2};
	}
	else if(argc == 3)
	{
		auto const address = boost::asio::ip::make_address(argv[1], ec);
		if(ec)
		{
			std::cerr << "ERROR! [" << argv[1] << "] Invalid address\n";
			return EXIT_FAILURE;
		}

		auto port1 = static_cast<unsigned short>(std::atoi(argv[2]));
		auto port2 = static_cast<unsigned short>(port1 + 1);
		ep1 = boost::asio::ip::udp::endpoint{address, port1};
		ep2 = boost::asio::ip::udp::endpoint{address, port2};
	}
	else
	{
		std::cerr << "Error! Use:\n\t" << argv[0] << " [<addr>] <port>\n";
		return EXIT_FAILURE;
	}

	/**
	 * UDP Code
	 */
	boost::asio::io_context ioc;

	My_Async::UDP::Simple_Container_Server<udp_echo_server> servers(ioc);
	servers.open(ep1, ec);
	if(ec)
	{
		std::cout << "Error[open]: " << ec.value() << ": " << ec.message() << "\n";
		return EXIT_FAILURE;
	}

	servers.open(ep2, ec);
	if(ec)
	{
		std::cout << "Error[open]: " << ec.value() << ": " << ec.message() << "\n";
		return EXIT_FAILURE;
	}

	ioc.run();

	return EXIT_SUCCESS;
}
