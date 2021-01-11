#ifndef WEBSOCKET_LISTENER_BASE_TEST_HPP__
#define WEBSOCKET_LISTENER_BASE_TEST_HPP__

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>

#include <type_traits>
#include <iostream>
#include "types/traits.hpp"

namespace My_Async{
namespace Websocket{

// Accepts incoming connections and launches the sessions
template<typename Derived, bool MakeStrand>
class Listener_Base{
	Derived&
	derived(){
		return static_cast<Derived&>(*this);
	}

protected:
	boost::asio::io_context& ioc_;
    boost::asio::ip::tcp::acceptor acceptor_;

    //Check if the session is SSL. If not, exclude ctx_ attriute
//    using ssl_context = my_traits::exclude_attr_if<
//    		WebsocketSession::use_ssl, boost::asio::ssl::context&>;
//	ssl_context ctx_;
public:
    Listener_Base(
		boost::asio::io_context& ioc,
		boost::asio::ip::tcp::endpoint endpoint)
        : ioc_(ioc)
        , acceptor_(boost::asio::make_strand(ioc))
	{
        init(endpoint);
    }

//    Listener_Base(
//			boost::asio::io_context& ioc,
//			ssl_context ctx,
//			boost::asio::ip::tcp::endpoint endpoint)
//			: ioc_(ioc), acceptor_(boost::asio::make_strand(ioc))
//			, ctx_(ctx)
//	{
//        init(endpoint);
//	}

    virtual ~Listener_Base(){}

    // Start accepting incoming connections
    void
    run(){
        do_accept();
    }

protected:
    void init(boost::asio::ip::tcp::endpoint& endpoint){
    	boost::system::error_code ec;

		// Open the acceptor
		acceptor_.open(endpoint.protocol(), ec);
		if(ec){
			fail(ec, "open");
			return;
		}

		// Allow address reuse
		acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
		if(ec){
			fail(ec, "set_option");
			return;
		}

		// Bind to the server address
		acceptor_.bind(endpoint, ec);
		if(ec){
			fail(ec, "bind");
			return;
		}
		// Start listening for connections
		acceptor_.listen(
			boost::asio::socket_base::max_listen_connections, ec);
		if(ec) {
			fail(ec, "listen");
			return;
		}
    }

    virtual void
    fail(boost::system::error_code ec, char const* what){
        std::cerr << what << ": " << ec.message() << "\n";
    }

    void
    do_accept(){
        // The new connection gets its own strand
    	if constexpr (MakeStrand){
    			// The new connection gets its own strand
    			acceptor_.async_accept(
    				boost::asio::make_strand(ioc_),
    				boost::beast::bind_front_handler(
    					&Listener_Base<Derived, MakeStrand>::on_accept,
    					derived().shared_from_this()));
    		} else {
    			//No strand
    			acceptor_.async_accept(
    				ioc_,
    				boost::beast::bind_front_handler(
    					&Listener_Base<Derived, MakeStrand>::on_accept,
						derived().shared_from_this()));
    		}
    }

    virtual void on_accept(boost::system::error_code ec,
    	    			boost::asio::ip::tcp::socket socket) = 0;
//    {
//		if(ec){
//			fail(ec, "accept");
//		} else {
//			if constexpr (WebsocketSession::use_ssl == false){
//				//Plain
//				std::make_shared<WebsocketSession>(std::move(socket))->run();
//			} else {
//				//SSL
//				std::make_shared<WebsocketSession>(std::move(socket), ctx_)->run();
//			}
//		}
//
//		// Accept another connection
//		do_accept();
//	}
};

/**
 * Classe que MUST ser specializada abaixo
 */
template<typename ...Dummy>
class Listener;

template<typename WebsocketSession>
class Listener<WebsocketSession> :
		public Listener_Base<Listener<WebsocketSession>, true>
		, public std::enable_shared_from_this<Listener<WebsocketSession>>{
		using ssl_context = my_traits::exclude_attr_if<
				WebsocketSession::use_ssl, boost::asio::ssl::context&>;
	public:
		using Listener_Base<Listener<WebsocketSession>, true>::Listener_Base;
	    Listener(
				boost::asio::io_context& ioc,
				ssl_context ctx,
				boost::asio::ip::tcp::endpoint endpoint)
				:
				Listener_Base<Listener<WebsocketSession>, true>(ioc)
//				ioc_(ioc), acceptor_(boost::asio::make_strand(ioc))
				, ctx_(ctx)
		{
	    	Listener_Base<Listener<WebsocketSession>, true>::
				init(endpoint);
		}

	    virtual ~Listener(){}

	protected:
		ssl_context ctx_;

	    virtual void on_accept(boost::system::error_code ec,
	    	    			boost::asio::ip::tcp::socket socket){
			if(ec){
		    	Listener_Base<Listener<WebsocketSession>, true>::
					fail(ec, "accept");
			} else {
				if constexpr (WebsocketSession::use_ssl == false){
					//Plain
					std::make_shared<WebsocketSession>(std::move(socket))->run();
				} else {
					//SSL
					std::make_shared<WebsocketSession>(std::move(socket), ctx_)->run();
				}
			}

			// Accept another connection
	    	Listener_Base<Listener<WebsocketSession>, true>::
				do_accept();
		}
};

//#include "detect_protocol_session.hpp"

//template<typename WebsocketPlain, typename WebsocketSSL, bool MakeStrand = true>
//class Listener_Dual :
//		public std::enable_shared_from_this<Listener_Dual<WebsocketPlain, WebsocketSSL, MakeStrand>>{
//    boost::asio::io_context& ioc_;
//    boost::asio::ip::tcp::acceptor acceptor_;
//    boost::asio::ssl::context& ctx_;
//public:
//    Listener_Dual(
//			boost::asio::io_context& ioc,
//			boost::asio::ssl::context& ctx,
//			boost::asio::ip::tcp::endpoint endpoint)
//			: ioc_(ioc), acceptor_(boost::asio::make_strand(ioc))
//			, ctx_(ctx)
//	{
//        init(endpoint);
//	}
//
//    virtual ~Listener_Dual(){}
//
//    // Start accepting incoming connections
//    void
//    run(){
//        do_accept();
//    }
//
//protected:
//    void init(boost::asio::ip::tcp::endpoint& endpoint){
//    	boost::system::error_code ec;
//
//		// Open the acceptor
//		acceptor_.open(endpoint.protocol(), ec);
//		if(ec){
//			fail(ec, "open");
//			return;
//		}
//
//		// Allow address reuse
//		acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
//		if(ec){
//			fail(ec, "set_option");
//			return;
//		}
//
//		// Bind to the server address
//		acceptor_.bind(endpoint, ec);
//		if(ec){
//			fail(ec, "bind");
//			return;
//		}
//		// Start listening for connections
//		acceptor_.listen(
//			boost::asio::socket_base::max_listen_connections, ec);
//		if(ec) {
//			fail(ec, "listen");
//			return;
//		}
//    }
//
//    virtual void
//    fail(boost::system::error_code ec, char const* what){
//        std::cerr << what << ": " << ec.message() << "\n";
//    }
//
//    void
//    do_accept(){
//        // The new connection gets its own strand
//    	if constexpr (MakeStrand){
//    			// The new connection gets its own strand
//    			acceptor_.async_accept(
//    				boost::asio::make_strand(ioc_),
//    				boost::beast::bind_front_handler(
//    					&Listener_Dual<WebsocketPlain, WebsocketSSL, MakeStrand>::on_accept,
//    					this->shared_from_this()));
//    		} else {
//    			//No strand
//    			acceptor_.async_accept(
//    				ioc_,
//    				boost::beast::bind_front_handler(
//    					&Listener_Dual<WebsocketPlain, WebsocketSSL, MakeStrand>::on_accept,
//						this->shared_from_this()));
//    		}
//    }
//
//    virtual void on_accept(boost::system::error_code ec,
//    	    			boost::asio::ip::tcp::socket socket){
//		if(ec){
//			fail(ec, "accept");
//		}else{
//			// Create the session and run it
//			std::make_shared<My_Async::Websocket::Detect_Session<WebsocketPlain, WebsocketSSL>>(std::move(socket), ctx_)->run();
//		}
//
//		// Accept another connection
//		do_accept();
//	}
//};

}//Websocket
}//My_Async

#endif /* WEBSOCKET_LISTENER_BASE_TEST_HPP__ */
