#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>

#include <string>

#include "my_async.hpp"

/**
 * Tests:
 * With SSL:
 * openssl s_client -connect 127.0.0.1:<port> -CAfile <path to cert>/mydomain.com.crt
 * Without SSL:
 * nc 127.0.0.1 <port>
 */

#define USE_TCP_SSL

#ifdef USE_TCP_SSL
#include "server_certificate.hpp"
#endif

#define ADDRESS		"0.0.0.0"

using namespace My_Async;

template<bool UseSSL>
using Session_CB = TCP::Session_Static_Callback<UseSSL, std::string>;

template<bool UseSSL>
void on_error(std::shared_ptr<Session_CB<UseSSL>>, boost::system::error_code ec, char const* what){
	std::cerr << what << "[" << ec.value() << "]: " << ec.message() << "\n";
}

template<bool UseSSL>
void on_close(std::shared_ptr<Session_CB<UseSSL>>, boost::system::error_code ec){
	std::cout << "Closing: " << ec.message() << "\n";
}

template<bool UseSSL>
void on_open(std::shared_ptr<Session_CB<UseSSL>>){
	std::cout << "Opening\n";
}

template<bool UseSSL>
void on_message(std::shared_ptr<Session_CB<UseSSL>> socket, std::string data){
	std::cout << "data: " << data << "\n";
	socket->write(data);
}

template<bool UseSSL>
void init_cb(){
	Session_CB<UseSSL>::set_on_open(on_open<UseSSL>);
	Session_CB<UseSSL>::set_on_error(on_error<UseSSL>);
	Session_CB<UseSSL>::set_on_message(on_message<UseSSL>);
	Session_CB<UseSSL>::set_on_close(on_close<UseSSL>);
}

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
    init_cb<true>();

    boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};
    load_server_certificate(ctx);

    My_Async::Listener::make_listener<Session_CB<true>>(ioc, ctx,
				boost::asio::ip::tcp::endpoint{address, port})->run();
#else
    init_cb<false>();

    My_Async::Listener::make_listener<Session_CB<false>>(ioc,
    				boost::asio::ip::tcp::endpoint{address, port})->run();
#endif
    ioc.run();

  return 0;
}
