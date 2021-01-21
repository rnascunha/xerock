#ifndef MY_ASYNC_TCP_CLIENT_BASE_HPP__
#define MY_ASYNC_TCP_CLIENT_BASE_HPP__

#include <type_traits>

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
class Client_Base {
	private:
		Derived&
		derived()
		{
			return static_cast<Derived&>(*this);
		}

		using self_type = Client_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>;

	public:
		static constexpr const bool tcp_client = true;
		static constexpr const bool use_ssl = UseSSL;

#if USE_SSL == 1
		//SSL contructors
		explicit
		Client_Base(
			boost::asio::io_context& ioc,
			boost::asio::ssl::context& ctx)
			: stream_(ioc, ctx){}
#endif
		//Plain contructor
		explicit
		Client_Base(boost::asio::io_context& ioc)
		  : stream_(ioc){}

		virtual ~Client_Base(){}

		void open(tcp::endpoint ep) noexcept;
		template<typename Callback>
		void open(tcp::endpoint ep, Callback cb) noexcept;

		void close() noexcept;

		void write(std::shared_ptr<OutContainer const> const& data) noexcept;
		void write(OutContainer const data) noexcept;

		bool keep_alive() const noexcept;
		void keep_alive(bool enable) noexcept;

		void keep_alive(int32_t idle, int32_t count, int32_t interval, bool set = true) noexcept;

		tcp::endpoint endpoint() const;
		tcp::endpoint local_endpoint() const;

	protected:
#if USE_SSL == 1
		//Do SSL handshake
		void on_handshake(boost::system::error_code);
		template<typename Callback>
		void on_handshake_cb(boost::system::error_code, Callback);
#endif
		void opened(const boost::system::error_code&) noexcept;
		template<typename Callback>
		void opened_cb(const boost::system::error_code&, Callback) noexcept;

		void closing() noexcept;

		void do_read() noexcept;
		void on_read(boost::system::error_code, std::size_t) noexcept;

		void fail(boost::system::error_code ec, char const* what) noexcept;
		virtual void read_handler(InContainer data) noexcept = 0;

		virtual void on_open() noexcept{}
		virtual void on_error(boost::system::error_code, char const*) noexcept{}
		virtual void on_close(boost::system::error_code) noexcept{}

		void do_write() noexcept;
		void writing(std::shared_ptr<OutContainer const> const&) noexcept;
		void on_write(boost::system::error_code, std::size_t) noexcept;

#if USE_SSL == 1
		using Stream = typename std::conditional<UseSSL == false,
			tcp::socket,
			boost::asio::ssl::stream<tcp::socket>>::
			type;
#else
		using Stream = tcp::socket;
#endif
		Stream stream_;

		std::vector<std::shared_ptr<OutContainer const>> queue_;
		InContainer buffer_;
};

}//TCP
}//My_Async

#include "impl/client_base_impl.hpp"

#endif /* MY_ASYNC_TCP_CLIENT_BASE_HPP__ */
