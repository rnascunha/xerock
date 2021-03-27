#ifndef MY_ASYNC_TCP_SERVER_SESSION_HPP__
#define MY_ASYNC_TCP_SERVER_SESSION_HPP__

#include <boost/asio.hpp>
#if USE_SSL == 1
#include <boost/asio/ssl.hpp>
#endif /* USE_SSL */
using boost::asio::ip::tcp;

namespace My_Async{
namespace TCP{

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer = InContainer,
		std::size_t ReadBufferSize = 1024>
class Session_Base
{
	private:
		Derived&
		derived(){
			return static_cast<Derived&>(*this);
		}

		using self_type = Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>;

	public:
		static constexpr const bool tcp_session = true;
		static constexpr const bool use_ssl = UseSSL;
		using shareable_data_type = struct{};

#if USE_SSL == 1
		//SSL contructors
		explicit
		Session_Base(
			boost::asio::basic_stream_socket<boost::asio::ip::tcp>&& socket,
			boost::asio::ssl::context& ctx)
			: stream_(std::move(socket), ctx){}
#endif

		//Plain contructor
		explicit
		Session_Base(
			boost::asio::basic_stream_socket<boost::asio::ip::tcp>&& stream)
		  : stream_(std::move(stream)){}

		virtual ~Session_Base(){}

		virtual void run() noexcept;

		void close();

		virtual void write(std::shared_ptr<OutContainer const> const& data) noexcept;
		virtual void write(OutContainer const data) noexcept;

		bool keep_alive() const noexcept;
		void keep_alive(bool enable) noexcept;

		void keep_alive(int32_t idle, int32_t count, int32_t interval, bool set = true) noexcept;

		boost::asio::ip::tcp::endpoint endpoint() const;
		boost::asio::ip::tcp::endpoint local_endpoint() const;
	protected:
#if USE_SSL == 1
		using Stream = typename std::conditional<UseSSL == false,
			tcp::socket,
			boost::asio::ssl::stream<tcp::socket>>::
			type;
#else
		using Stream = tcp::socket;
#endif

		Stream stream_;

		void closing() noexcept;

		/*Essa funcao foi criada para desambiguar a funcao write chamada em um bind...*/
		virtual void write_share(std::shared_ptr<OutContainer const> const& data) noexcept{
			write(data);
		}

		void on_run() noexcept;
#if USE_SSL == 1
		//Do SSL handshake
		void on_handshake(boost::system::error_code ec);
#endif
		virtual void do_accept() noexcept;

		virtual void fail(boost::system::error_code ec, char const* what) noexcept;

		virtual void do_read() noexcept;
		virtual void on_read(boost::system::error_code ec, std::size_t bytes_transferred) noexcept;
		virtual void read_handler(InContainer data) noexcept = 0;

		virtual void do_write() noexcept;
		virtual void writing(std::shared_ptr<OutContainer const> const& data) noexcept;
		virtual void on_write(boost::system::error_code ec, std::size_t bytes_transferred) noexcept;

		virtual void on_open() noexcept{}
		virtual void on_error(boost::system::error_code, char const*) noexcept{}
		virtual void on_close(boost::system::error_code) noexcept{}

		std::vector<std::shared_ptr<OutContainer const>> queue_;
		InContainer buffer_;
};

}//TCP
}//My_Async

#include "impl/session_base_impl.hpp"

#endif /* MY_ASYNC_TCP_SERVER_SESSION_HPP__ */
