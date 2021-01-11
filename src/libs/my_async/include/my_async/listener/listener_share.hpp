#ifndef SESSION_LISTENER_SHARE_HPP__
#define SESSION_LISTENER_SHARE_HPP__

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#if USE_SSL == 1
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#endif

#include <type_traits>
#include <iostream>

#include "types/traits.hpp"
#include "my_async/traits.hpp"

#include <memory>
//#include <unordered_set>
#include "util/shareable.hpp"

namespace My_Async{
namespace Listener{

// Accepts incoming connections and launches the sessions
template<typename Session, bool MakeStrand = true>
class Listener_Share :
		public std::enable_shared_from_this<Listener_Share<Session, MakeStrand>>
{
		boost::asio::io_context& ioc_;
		boost::asio::ip::tcp::acceptor acceptor_;

#if USE_SSL == 1
		//Check if the session is SSL. If not, exclude ctx_ attriute
		using ssl_context = my_traits::exclude_attr_if<
				Session::use_ssl, boost::asio::ssl::context&>;
		ssl_context ctx_;
#else
		using ssl_context = struct{};
#endif
//		std::shared_ptr<std::unordered_set<Session*>> share_;
		std::shared_ptr<Shareable<Session>> share_;
	public:
		static constexpr const bool use_ssl = Session::use_ssl;

		Listener_Share(
			boost::asio::io_context& ioc)
			: ioc_(ioc)
			, acceptor_(boost::asio::make_strand(ioc))
		{
			share_ = std::make_shared<Shareable<Session>>();
		}

#if USE_SSL == 1
		Listener_Share(
				boost::asio::io_context& ioc,
				ssl_context ctx)
				: ioc_(ioc), acceptor_(boost::asio::make_strand(ioc))
				, ctx_(ctx){}
#endif

		virtual ~Listener_Share(){}

		void close(boost::system::error_code& ec) noexcept
		{
			acceptor_.cancel(ec);
			if(ec) return fail(ec, "cancel");
			acceptor_.close(ec);
			if(ec) return fail(ec, "close");
		}

		void
		init(boost::asio::ip::tcp::endpoint endpoint, boost::system::error_code& ec) noexcept
		{
			// Open the acceptor
			acceptor_.open(endpoint.protocol(), ec);
			if(ec)
				return fail(ec, "open");

			// Allow address reuse
			acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
			if(ec) return fail(ec, "set_option");

			// Bind to the server address
			acceptor_.bind(endpoint, ec);
			if(ec) return fail(ec, "bind");

			// Start listening for connections
			acceptor_.listen(
				boost::asio::socket_base::max_listen_connections, ec);
			if(ec) return fail(ec, "listen");

			do_accept();
		}

	protected:

		virtual void
		fail(boost::system::error_code ec, char const* what) noexcept
		{
			std::cerr << what << ": " << ec.message() << "\n";
		}

		void
		do_accept() noexcept
		{
			// The new connection gets its own strand
			if constexpr (MakeStrand){
				// The new connection gets its own strand
				acceptor_.async_accept(
					boost::asio::make_strand(ioc_),
					boost::beast::bind_front_handler(
						&Listener_Share<Session, MakeStrand>::on_accept,
						this->shared_from_this()));
			} else {
				//No strand
				acceptor_.async_accept(
					ioc_,
					boost::beast::bind_front_handler(
						&Listener_Share<Session, MakeStrand>::on_accept,
						this->shared_from_this()));
			}
		}

		virtual void on_accept(boost::system::error_code ec,
							boost::asio::ip::tcp::socket socket) noexcept
		{
			if(ec)
				return fail(ec, "accept");

			if constexpr (Session::use_ssl == false){
				//Plain
				std::make_shared<Session>(std::move(socket), share_)->run();
			} else {
				//SSL
#if USE_SSL == 1
				std::make_shared<Session>(std::move(socket), ctx_, share_)->run();
#endif
			}

			// Accept another connection
			do_accept();
		}
};

}//Listener
}//My_Async

#endif /* SESSION_LISTENER_SHARE_HPP__ */
