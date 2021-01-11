#ifndef SESSION_LISTENER_DUAL_HPP__
#define SESSION_LISTENER_DUAL_HPP__

#if USE_SSL == 1

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>

#include "my_async/traits.hpp"
#include "detect_protocol_session.hpp"

namespace My_Async{
namespace Listener{

template<typename SessionPlain, typename SessionSSL, bool MakeStrand = true>
class Listener_Dual :
		public std::enable_shared_from_this<Listener_Dual<SessionPlain, SessionSSL, MakeStrand>>{

//	static_assert(My_Async::is_websocket_session<WebsocketPlain>
//			&& My_Async::is_websocket_session<WebsocketSSL>,
//			"WebsocketPlain and WebsocketSSL must be a class inherent from Websocket::Session_Base");

	boost::asio::io_context& ioc_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ssl::context& ctx_;
public:
    Listener_Dual(
			boost::asio::io_context& ioc,
			boost::asio::ssl::context& ctx,
			boost::asio::ip::tcp::endpoint endpoint)
			: ioc_(ioc), acceptor_(boost::asio::make_strand(ioc))
			, ctx_(ctx)
	{
        init(endpoint);
	}

    virtual ~Listener_Dual(){}

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
    					&Listener_Dual<SessionPlain, SessionSSL, MakeStrand>::on_accept,
    					this->shared_from_this()));
    		} else {
    			//No strand
    			acceptor_.async_accept(
    				ioc_,
    				boost::beast::bind_front_handler(
    					&Listener_Dual<SessionPlain, SessionSSL, MakeStrand>::on_accept,
						this->shared_from_this()));
    		}
    }

    virtual void on_accept(boost::system::error_code ec,
    	    			boost::asio::ip::tcp::socket socket){
		if(ec){
			fail(ec, "accept");
		}else{
			// Create the session and run it
			std::make_shared<Detect_Session<SessionPlain, SessionSSL>>(std::move(socket), ctx_)->run();
		}

		// Accept another connection
		do_accept();
	}
};

}//Listener
}//My_Async

#endif /* USE_SSL == 1 */

#endif /* SESSION_LISTENER_DUAL_HPP__ */
