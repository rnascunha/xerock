#ifndef WEBSOCKET_SESSION_BASE_ASYNC_HPP__
#define WEBSOCKET_SESSION_BASE_ASYNC_HPP__

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#if USE_SSL == 1
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#endif


namespace My_Async{
namespace Websocket{

/**
 * "Desambiguar" os metodos write_share e write_share2
 */
template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer = InContainer,
		std::size_t ReadBufferSize = 1024>
class Session_Base{
	private:
		Derived&
		derived(){
			return static_cast<Derived&>(*this);
		}

		using self_type = Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>;
	public:
		static constexpr const bool websocket_session = true;
		static constexpr const bool use_ssl = UseSSL;
		using shareable_data_type = struct{};

		//Plain constructors
	    explicit
		Session_Base(
			boost::asio::basic_stream_socket<boost::asio::ip::tcp>&& socket)
		  : stream_(std::move(socket)){}

	    /* construtor usando quando o accpet vem da classe http usado no modo dual (plain e ssl) */
		explicit
		Session_Base(
			boost::beast::basic_stream<boost::asio::ip::tcp>&& socket)
	          : stream_(std::move(socket)){}

#if USE_SSL == 1
		//SSL contructors
		explicit
		Session_Base(
			boost::asio::basic_stream_socket<boost::asio::ip::tcp>&& socket,
			boost::asio::ssl::context& ctx)
			: stream_(std::move(socket), ctx){}

		explicit
		Session_Base(
			boost::beast::ssl_stream<boost::beast::basic_stream<boost::asio::ip::tcp>>&& stream)
		  : stream_(std::move(stream)){}
#endif
		virtual ~Session_Base(){}

		/**
		 * Este run acontece quando o upgrade (http -> websocket) é feito por uma
		 * classe anterior
		 */
		template<class Body, class Allocator>
		void
		run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req);

		virtual void run() noexcept;

		virtual void close(const boost::beast::websocket::close_reason&) noexcept;

		virtual void write(std::shared_ptr<OutContainer const> const& data) noexcept;
		virtual void write(std::shared_ptr<OutContainer const> const& data, bool text) noexcept;
		virtual void write(OutContainer const data) noexcept;
		virtual void write(OutContainer const data, bool text) noexcept;

		/*Essa funcao foi criada para desambiguar a funcao write chamada em um bind...*/
		virtual void write_share(std::shared_ptr<OutContainer const> const& data) noexcept{
			write(data);
		}

		virtual void write_share2(std::shared_ptr<OutContainer const> const& data, bool text) noexcept{
			this->text(text);
			write(data);
		}

		void binary(bool b) noexcept;
		void text(bool b) noexcept;
		bool binary() noexcept;
		bool text() noexcept;

	protected:
#if USE_SSL == 1
		using Stream = typename std::conditional<UseSSL == false,
			boost::beast::websocket::stream<boost::beast::tcp_stream>,
			boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>>>::
			type;
#else
		using Stream = boost::beast::websocket::stream<boost::beast::tcp_stream>;
#endif

		Stream stream_;

		void on_run() noexcept;
#if USE_SSL == 1
		//Do SSL handshake
		void on_handshake(boost::system::error_code ec);
#endif
		virtual void fail(boost::system::error_code ec, char const* what) noexcept = 0;

		virtual void on_accept(boost::system::error_code ec);

		virtual void do_read() noexcept;
		virtual void on_read(boost::system::error_code ec, std::size_t bytes_transferred) noexcept;
		virtual void read_handler(InContainer data) noexcept = 0;

		virtual void do_write() noexcept;
		virtual void writing(std::shared_ptr<OutContainer const> const& data) noexcept;
		virtual void on_write(boost::system::error_code ec, std::size_t bytes_transferred) noexcept;

		virtual void set_run_options();

		virtual void closed(boost::beast::error_code const&);

		virtual void on_open() noexcept{}
		virtual void on_close(boost::beast::error_code) noexcept{}

		std::vector<std::shared_ptr<OutContainer const>> queue_;
		InContainer buffer_;
};

}//Websocket
}//My_Async

#include "impl/session_base_impl.hpp"

#endif /* WEBSOCKET_SESSION_BASE_ASYNC_HPP__ */
