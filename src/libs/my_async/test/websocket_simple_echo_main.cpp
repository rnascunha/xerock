#include <iostream>
#include <boost/asio.hpp>

#define USE_LISTENER_HELPER

#if !defined(USE_LISTENER_HELPER)
#include <memory>
#endif

#define USE_WEBSOCKET_PLAIN
#define USE_WEBSOCKET_SSL

#ifdef USE_WEBSOCKET_SSL
#include "server_certificate.hpp"
#endif

#include "my_async.hpp"

#define ADDRESS		"0.0.0.0"
#define PORT		8080

template<bool UseSSL>
class Simple_Echo :
		public My_Async::Websocket::Session_Base<Simple_Echo<UseSSL>, UseSSL, std::string>
		, public std::enable_shared_from_this<Simple_Echo<UseSSL>>{
	public:
		using My_Async::Websocket::Session_Base<Simple_Echo<UseSSL>, UseSSL, std::string>::
				Session_Base;

	protected:
		void fail(boost::system::error_code ec, char const* what) noexcept{
			std::cerr << what << ": " << ec.message() << "\n";
		}

		void read_handler(std::string data) noexcept{
			std::cout << "data: " << data << "\n";
			My_Async::Websocket::Session_Base<Simple_Echo<UseSSL>, UseSSL, std::string>::
				write(data);
		}
};

int main(){
	// The io_context is required for all I/O
	boost::asio::io_context ioc;

	auto const address = boost::asio::ip::make_address(ADDRESS);
	const unsigned int port = PORT;

#ifdef USE_WEBSOCKET_SSL
	boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};
	load_server_certificate(ctx);
#endif

//Plain
#if defined(USE_WEBSOCKET_PLAIN) && !defined(USE_WEBSOCKET_SSL)
#ifdef USE_LISTENER_HELPER
	My_Async::Websocket::
			make_listener<Simple_Echo<false>>(
				ioc,
				address, port)->run();
#else/* USE_LISTENER_HELPER */
    std::make_shared<My_Async::Websocket::
    		Listener_Base<Simple_Echo<false>>>
    				(ioc,
    				boost::asio::ip::tcp::endpoint{address, port})->run();
#endif/* USE_LISTENER_HELPER */
#endif

//SSL
#if !defined(USE_WEBSOCKET_PLAIN) && defined(USE_WEBSOCKET_SSL)
#ifdef USE_LISTENER_HELPER
    My_Async::Websocket::
    		make_listener<Simple_Echo<true>>(
    			ioc,
				ctx,
    			address, port)->run();
#else/* USE_LISTENER_HELPER */
	std::make_shared<My_Async::Websocket::
		Listener_Base<Simple_Echo<true>>>
				(ioc,
				ctx,
				boost::asio::ip::tcp::endpoint{address, port})->run();
#endif/* USE_LISTENER_HELPER */
#endif

//Plain and SSL
#if defined(USE_WEBSOCKET_PLAIN) && defined(USE_WEBSOCKET_SSL)
#ifdef USE_LISTENER_HELPER
    My_Async::Websocket::
		make_listener<Simple_Echo>(
			ioc,
			ctx,
			address, port)->run();
//	  Ou:
//    My_Async::Websocket::
//		make_listener<Simple_Echo<false>, Simple_Echo<true>>(
//			ioc,
//			ctx,
//			address, port)->run();
#else/* USE_LISTENER_HELPER */
	    std::make_shared<My_Async::Websocket::
		Listener_Dual<Simple_Echo<false>, Simple_Echo<true>>>
				(ioc,
				ctx,
				boost::asio::ip::tcp::endpoint{address, port})->run();

#endif/* USE_LISTENER_HELPER */
#endif
	// Create and launch a listening port
	boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
	signals.async_wait(
		[&ioc](boost::system::error_code const&, int){
			ioc.stop();
		});

	ioc.run();
	return EXIT_SUCCESS;
}
