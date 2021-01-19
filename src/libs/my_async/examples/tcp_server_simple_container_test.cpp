#include "my_async.hpp"

#include <memory>
#include <iostream>

#undef USE_SSL
//#define USE_SSL 1

#if USE_SSL == 1
#include "server_certificate.hpp"
#endif /* USE_SSL == 1 */

static const char* addr = "0.0.0.0";
static unsigned short port = 8089;

template<bool UseSSL>
using Session = My_Async::TCP::Session_Static_Share_Static_Callback<UseSSL, std::string>;

template<bool UseSSL>
static void on_error(std::shared_ptr<Session<UseSSL>> sock, boost::system::error_code ec, char const* what){
	std::cerr << sock->endpoint().address().to_string() << ":" << sock->endpoint().port() << " ";
	std::cerr << what << ": " << ec.message() << "\n";
}

template<bool UseSSL>
static void on_close(std::shared_ptr<Session<UseSSL>> sock, boost::system::error_code){
	std::cout << sock->endpoint().address().to_string() << ":" << sock->endpoint().port() << " Closed\n";
}

template<bool UseSSL>
static void on_open(std::shared_ptr<Session<UseSSL>> sock){
	std::cout << sock->endpoint().address().to_string() << ":" << sock->endpoint().port() << " Opened\n";
}

template<bool UseSSL>
static void on_message(std::shared_ptr<Session<UseSSL>> sock, Byte_Array data){
	std::cout << sock->endpoint().address().to_string() << ":" << sock->endpoint().port() << " Data: ";
	std::cout << data.to_string() << "\n";
}

template<bool UseSSL>
static void init_cb(){
	Session<UseSSL>::set_on_open(on_open<UseSSL>);
	Session<UseSSL>::set_on_error(on_error<UseSSL>);
	Session<UseSSL>::set_on_message(on_message<UseSSL>);
	Session<UseSSL>::set_on_close(on_close<UseSSL>);
}

int main(){
	boost::system::error_code ec;
	auto const address = boost::asio::ip::make_address(addr, ec);

	// The io_context is required for all I/O
	boost::asio::io_context ioc;

#if USE_SSL == 1
	boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};
	// This holds the self-signed certificate used by the server
	load_server_certificate(ctx);

	init_cb<true>();
    My_Async::Listener::Simple_Container<
		My_Async::Listener::Listener_Base<Session<true>>> tcp_servers(ioc, ctx);
#else
	init_cb<false>();
    My_Async::Listener::Simple_Container<
		My_Async::Listener::Listener_Base<Session<false>>> tcp_servers(ioc);
#endif
    boost::asio::ip::tcp::endpoint ep1{address, port};
    tcp_servers.open(ep1, ec);
    if(ec){
    	std::cerr << "Error! ep1\n";
    	return EXIT_FAILURE;
    }
    std::cout << "Opened ep1\n";

    boost::asio::ip::tcp::endpoint ep2{address, static_cast<unsigned short>(port + 1)};
	tcp_servers.open(ep2, ec);
	if(ec){
		std::cerr << "Error! ep2\n";
		return EXIT_FAILURE;
	}
    std::cerr << "Opened ep2\n";

    ioc.run();
}
