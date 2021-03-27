#include <iostream>
#include <boost/asio.hpp>

#define USE_LISTENER_HELPER

#if !defined(USE_LISTENER_HELPER)
#include <memory>
#endif

//#define USE_WEBSOCKET_PLAIN
#define USE_WEBSOCKET_SSL

#ifdef USE_WEBSOCKET_SSL
#include "server_certificate.hpp"
#endif

#include "my_async.hpp"

#define ADDRESS		"0.0.0.0"
#define PORT		8080

template<bool UseSSL>
using Echo_All = My_Async::Websocket::Session_Static_Cb_Static_Shareable<UseSSL, std::string>;

template<bool UseSSL>
static void on_open(std::shared_ptr<Echo_All<UseSSL>>){
	std::cout << "Open\n";
}

template<bool UseSSL>
static void on_close(std::shared_ptr<Echo_All<UseSSL>>,
		boost::system::error_code ec){
	std::cout << "Close: " << ec.message() << "\n";
}

template<bool UseSSL>
static void on_message(std::shared_ptr<Echo_All<UseSSL>> ws,
		std::string const& data){
	std::cout << "data: " << data << "\n";
	ws->write_all(data);
}

template<bool UseSSL>
static void on_error(std::shared_ptr<Echo_All<UseSSL>>,
		boost::system::error_code ec,
		char const* what){
	std::cerr << what << ": " << ec.message() << "\n";
}

int main(){
	// The io_context is required for all I/O
	boost::asio::io_context ioc;

#ifdef USE_WEBSOCKET_PLAIN
	Echo_All<false>::on_open(on_open<false>);
	Echo_All<false>::on_close(on_close<false>);
	Echo_All<false>::on_message(on_message<false>);
	Echo_All<false>::on_error(on_error<false>);
#endif

#ifdef USE_WEBSOCKET_SSL
	Echo_All<true>::on_open(on_open<true>);
	Echo_All<true>::on_close(on_close<true>);
	Echo_All<true>::on_message(on_message<true>);
	Echo_All<true>::on_error(on_error<true>);
#endif

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
			make_listener<Echo_All<false>>(
				ioc,
				address, port)->run();
#else/* USE_LISTENER_HELPER */
    std::make_shared<My_Async::Websocket::
    		Listener_Base<Echo_All<false>>>
    				(ioc,
    				boost::asio::ip::tcp::endpoint{address, port})->run();
#endif/* USE_LISTENER_HELPER */
#endif

//SSL
#if !defined(USE_WEBSOCKET_PLAIN) && defined(USE_WEBSOCKET_SSL)
#ifdef USE_LISTENER_HELPER
    My_Async::Websocket::
    		make_listener<Echo_All<true>>(
    			ioc,
				ctx,
    			address, port)->run();
#else/* USE_LISTENER_HELPER */
	std::make_shared<My_Async::Websocket::
		Listener_Base<Echo_All<true>>>
				(ioc,
				ctx,
				boost::asio::ip::tcp::endpoint{address, port})->run();
#endif/* USE_LISTENER_HELPER */
#endif

//Plain and SSL
#if defined(USE_WEBSOCKET_PLAIN) && defined(USE_WEBSOCKET_SSL)
#ifdef USE_LISTENER_HELPER
    My_Async::Websocket::
		make_listener<Echo_All<false>, Echo_All<true>>(
			ioc,
			ctx,
			address, port)->run();
#else/* USE_LISTENER_HELPER */
	    std::make_shared<My_Async::Websocket::
		Listener_Dual<Echo_All<false>, Echo_All<true>>>
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
