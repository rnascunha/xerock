#ifndef UDP_CLIENT_BASE_SESSION_HPP__
#define UDP_CLIENT_BASE_SESSION_HPP__

#include <boost/asio.hpp>

namespace My_Async{
namespace UDP{

template<typename Derived,
		typename InContainer,
		typename OutContainer = InContainer,
		std::size_t ReadBufferSize = 1024>
class Client_Base{
	private:
		Derived&
		derived()
		{
			return static_cast<Derived&>(*this);
		}

		using self_type = Client_Base<Derived, InContainer, OutContainer, ReadBufferSize>;

	public:
		Client_Base(boost::asio::io_context& ioc, const boost::asio::ip::udp::endpoint& ep);
		Client_Base(boost::asio::io_context& ioc);
		Client_Base(boost::asio::io_context& ioc, unsigned short port);

		~Client_Base(){}

		void open(boost::asio::ip::udp::endpoint const& endpoint) noexcept;

		void write(OutContainer const data) noexcept;
		void write(std::shared_ptr<OutContainer const> data) noexcept;

		void close(boost::system::error_code& ec) noexcept;

		boost::asio::ip::udp::endpoint endpoint() const;
		boost::asio::ip::udp::endpoint local_endpoint() const;
	protected:
		void do_read() noexcept;
		void on_read(boost::system::error_code ec, std::size_t bytes_transfered) noexcept;

		void fail(boost::system::error_code ec, char const* what) noexcept;

		void writing(std::shared_ptr<OutContainer const> const data) noexcept;
		void do_write() noexcept;
		void on_write(boost::system::error_code ec, std::size_t) noexcept;

		virtual void on_open() noexcept{}
		virtual void on_error(boost::system::error_code, char const*) noexcept{}
		virtual void on_close(boost::system::error_code) noexcept{}

		virtual void read_handler(InContainer data) noexcept = 0;

		boost::asio::ip::udp::socket socket_;
		boost::asio::ip::udp::endpoint endpoint_;

		std::vector<std::shared_ptr<OutContainer const>> queue_;
		InContainer buffer_;
};

}//UDP
}//My_Async

#include "impl/client_base_impl.hpp"

#endif /* UDP_CLIENT_BASE_SESSION_HPP__ */
