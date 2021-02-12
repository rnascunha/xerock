#ifndef UDP_SERVER_BASE_SESSION_HPP__
#define UDP_SERVER_BASE_SESSION_HPP__

#include <boost/asio.hpp>
#include <utility>

namespace My_Async{
namespace UDP{

template<typename Derived,
		typename InContainer,
		typename OutContainer = InContainer,
		std::size_t ReadBufferSize = 1024>
class Server_Base{
	private:
		Derived&
		derived()
		{
			return static_cast<Derived&>(*this);
		}

		using self_type = Server_Base<Derived, InContainer, OutContainer, ReadBufferSize>;

	public:
		Server_Base(boost::asio::io_context& ioc);
		~Server_Base(){}

		void open(const boost::asio::ip::udp::endpoint&,
				boost::system::error_code&) noexcept;
		void open(unsigned short port,
				boost::system::error_code&) noexcept;

		void write(const boost::asio::ip::udp::endpoint&, OutContainer const data) noexcept;
		void write(const boost::asio::ip::udp::endpoint&, std::shared_ptr<OutContainer const> data) noexcept;

		void close(boost::system::error_code& ec) noexcept;

		boost::asio::ip::udp::endpoint endpoint() const;
		boost::asio::ip::udp::endpoint local_endpoint() const;
	protected:
		void do_read() noexcept;
		void on_read(boost::system::error_code ec, std::size_t bytes_transfered) noexcept;

		void fail(boost::system::error_code ec, char const* what) noexcept;

		void writing(const boost::asio::ip::udp::endpoint, std::shared_ptr<OutContainer const> const) noexcept;
		void do_write() noexcept;
		void on_write(boost::system::error_code ec, std::size_t) noexcept;

		virtual void on_open() noexcept{}
		virtual void on_error(boost::system::error_code, char const*) noexcept{}
		virtual void on_close(boost::system::error_code) noexcept{}

		virtual void read_handler(InContainer data) noexcept = 0;

		boost::asio::ip::udp::socket socket_;
		boost::asio::ip::udp::endpoint endpoint_;

		std::vector<
			std::pair<
				boost::asio::ip::udp::endpoint,
				std::shared_ptr<OutContainer const>>> queue_;
		InContainer buffer_;
};

}//UDP
}//My_Async

#include "impl/server_base_impl.hpp"

#endif /* UDP_SERVER_BASE_SESSION_HPP__ */
