#include <iostream>
#include <boost/asio.hpp>

#define USE_LISTENER_HELPER

#if !defined(USE_LISTENER_HELPER)
#include <memory>
#endif

#define USE_WEBSOCKET_PLAIN
#define USE_WEBSOCKET_SSL

#ifdef USE_WEBSOCKET_SSL
#include "example/common/server_certificate.hpp"
#endif

#include "my_async.hpp"

#define ADDRESS		"0.0.0.0"
#define PORT		8080

template<bool UseSSL>
using Simple_Echo = My_Async::Websocket::Session_Static_Cb<UseSSL, std::string>;

template<bool UseSSL>
static void on_open(std::shared_ptr<Simple_Echo<UseSSL>>){
	std::cout << "Open\n";
}

template<bool UseSSL>
static void on_close(std::shared_ptr<Simple_Echo<UseSSL>>,
		boost::system::error_code ec){
	std::cout << "Close: " << ec.message() << "\n";
}

template<bool UseSSL>
static void on_message(std::shared_ptr<Simple_Echo<UseSSL>> ws,
		std::string const& data){
	std::cout << "data: " << data << "\n";
	ws->write(data);
}

template<bool UseSSL>
static void on_error(std::shared_ptr<Simple_Echo<UseSSL>>,
		boost::system::error_code ec,
		char const* what){
	std::cerr << what << ": " << ec.message() << "\n";
}

int main(){
	// The io_context is required for all I/O
	boost::asio::io_context ioc;

#ifdef USE_WEBSOCKET_PLAIN
	Simple_Echo<false>::on_open(on_open<false>);
	Simple_Echo<false>::on_close(on_close<false>);
	Simple_Echo<false>::on_message(on_message<false>);
	Simple_Echo<false>::on_error(on_error<false>);
#endif

#ifdef USE_WEBSOCKET_SSL
	Simple_Echo<true>::on_open(on_open<true>);
	Simple_Echo<true>::on_close(on_close<true>);
	Simple_Echo<true>::on_message(on_message<true>);
	Simple_Echo<true>::on_error(on_error<true>);
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
		make_listener<Simple_Echo<false>, Simple_Echo<true>>(
			ioc,
			ctx,
			address, port)->run();
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
