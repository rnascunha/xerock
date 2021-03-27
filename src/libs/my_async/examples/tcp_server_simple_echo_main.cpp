#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

#include <string>

#include "my_async.hpp"

//#define USE_TCP_SSL

#ifdef USE_TCP_SSL
#include "server_certificate.hpp"
#endif

#define ADDRESS		"0.0.0.0"
#define PORT		8089

template<bool UseSSL>
class Simple_Echo : public My_Async::TCP::Session_Base<Simple_Echo<UseSSL>, UseSSL, std::string>,
					public std::enable_shared_from_this<Simple_Echo<UseSSL>>
{
	using self_type = My_Async::TCP::Session_Base<Simple_Echo, UseSSL, std::string>;
	public:
		//Importing constructors
		using self_type::Session_Base;
	protected:
		void on_error(boost::system::error_code ec, char const* what) noexcept override{
			std::cerr << what << ": " << ec.message() << "\n";
		}

		void on_close(boost::system::error_code ec) noexcept override{
			std::cout << "Closing: " << ec.message() << "\n";
		}

		void on_open() noexcept override{
			std::cout << "Opening\n";
		}

		void read_handler(std::string data) noexcept override{
			std::cout << "data: " << data << "\n";
			self_type::write(data);
		}
};


int main(int argc, char* argv[])
{
	if (argc != 2)
    {
		std::cout << "Error:\n\t" <<basename(argv[0]) << " <port>\n";
        return EXIT_FAILURE;
    }

    boost::asio::io_context ioc;

    auto const address = boost::asio::ip::make_address(ADDRESS);
    unsigned short port = static_cast<unsigned short>(std::atoi(argv[1]));

#ifdef USE_TCP_SSL
    boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};
    load_server_certificate(ctx);

    My_Async::Listener::make_listener<Simple_Echo<true>>(ioc, ctx,
				boost::asio::ip::tcp::endpoint{address, port})->run();
#else
    My_Async::Listener::make_listener<Simple_Echo<false>>(ioc,
    				boost::asio::ip::tcp::endpoint{address, port})->run();
#endif
    ioc.run();

  return 0;
}
