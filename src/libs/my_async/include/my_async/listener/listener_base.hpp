#ifndef SESSION_LISTENER_BASE_HPP__
#define SESSION_LISTENER_BASE_HPP__

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#if USE_SSL == 1
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#endif /* USE_SSL == 1 */
#include <type_traits>
#include <iostream>

#include "types/traits.hpp"
#include "my_async/traits.hpp"

#include "util/shareable.hpp"

namespace My_Async{
namespace Listener{

// Accepts incoming connections and launches the sessions
template<typename Session, bool MakeStrand = true>
class Listener_Base :
		public std::enable_shared_from_this<Listener_Base<Session, MakeStrand>>
{
		static constexpr const bool is_shareable = my_traits::is_shareable<Session>::value;

		boost::asio::io_context& ioc_;
		boost::asio::ip::tcp::acceptor acceptor_;

#if USE_SSL == 1
		//Check if the session is SSL. If not, exclude ctx_ attribute
		using ssl_context = my_traits::exclude_attr_if<
				Session::use_ssl, boost::asio::ssl::context&>;
		ssl_context ctx_;
#else
		using ssl_context = struct{};
#endif
		using shareable = my_traits::exclude_attr_if<is_shareable,
				std::shared_ptr<Shareable<Session, typename Session::shareable_data_type>>>;
		shareable share_;
	public:
		static constexpr const bool use_ssl = Session::use_ssl;
		using Session_Type = Session;
		static constexpr const bool has_strand = MakeStrand;

		Listener_Base(
			boost::asio::io_context& ioc)
			: ioc_(ioc)
			, acceptor_(boost::asio::make_strand(ioc))
		{
			if constexpr(is_shareable)
				share_ = std::make_shared<Shareable<Session, typename Session::shareable_data_type>>();
		}
#if USE_SSL == 1
		Listener_Base(
				boost::asio::io_context& ioc,
				ssl_context ctx)
				: ioc_(ioc), acceptor_(boost::asio::make_strand(ioc))
				, ctx_(ctx)
		{
			if constexpr(is_shareable)
				share_ = std::make_shared<Shareable<Session, typename Session::shareable_data_type>>();
		}
#endif
		virtual ~Listener_Base(){}

		shareable
		get_share(){ return share_; }

		template<typename U = Session>
		typename std::enable_if<U::is_shareable,
					std::vector<boost::asio::ip::tcp::endpoint>>::type
		clients() const noexcept{
			std::vector<boost::asio::ip::tcp::endpoint> v;

			share_->exec_to_all([&v](Session* session){
				v.push_back(session->endpoint());
			});

			return v;
		}

		void
		close(boost::system::error_code&) noexcept
		{
			boost::asio::post(
				acceptor_.get_executor(),
					std::bind(&Listener_Base::closing,
							this->shared_from_this()));
		}

		template<typename U = Session,
				typename = typename std::enable_if<U::is_shareable>::type>
		void
		close(boost::system::error_code& ec, bool close_clients) noexcept
		{
			if(close_clients){
				share_->exec_to_all([this](Session* session){
					session->close();
				});
			}
			close(ec);
		}

		template<typename U = Session,
			typename = typename std::enable_if<U::is_shareable>::type>
		void
		close_clients(std::vector<boost::asio::ip::tcp::endpoint> eps)
		{
			share_->exec_to_all([&](Session* session){
				for(auto const& ep : eps){
					if(ep == session->endpoint()){
						session->close();
						break;
					}
				}
			});
		}

		template<typename OutContainer, typename U = Session,
			typename = typename std::enable_if<U::is_shareable>::type>
		void
		write(boost::asio::ip::tcp::endpoint client, std::shared_ptr<OutContainer const> const& data)
		{
			share_->exec_to_all([&](Session* session){
				if(client == session->endpoint())
					session->write(data);
			});
		}

		void
		init(boost::asio::ip::tcp::endpoint endpoint,
				boost::system::error_code& ec) noexcept
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

		void closing()
		{
			boost::system::error_code ec;
			acceptor_.cancel(ec);
			if(ec) return fail(ec, "cancel");
			acceptor_.close(ec);
			if(ec) return fail(ec, "close");
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
						&Listener_Base<Session, MakeStrand>::on_accept,
						this->shared_from_this()));
			} else {
				//No strand
				acceptor_.async_accept(
					ioc_,
					boost::beast::bind_front_handler(
						&Listener_Base<Session, MakeStrand>::on_accept,
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
				if constexpr(!is_shareable)
					std::make_shared<Session>(std::move(socket))->run();
				else
					std::make_shared<Session>(std::move(socket), share_)->run();
			} else {
#if USE_SSL == 1
				//SSL
				if constexpr(!is_shareable)
					std::make_shared<Session>(std::move(socket), ctx_)->run();
				else
					std::make_shared<Session>(std::move(socket), ctx_, share_)->run();
#endif
			}

			// Accept another connection
			do_accept();
		}
};

}//Listener
}//My_Async

#endif /* SESSION_LISTENER_BASE_HPP__ */
