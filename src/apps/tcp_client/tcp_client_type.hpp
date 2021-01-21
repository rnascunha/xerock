#ifndef TCP_CLIENT_APP_TYPE_DEF_HPP__
#define TCP_CLIENT_APP_TYPE_DEF_HPP__

#include "boost/asio.hpp"

#include "core/helpers.hpp"
#include "my_async.hpp"
#include "byte_array.h"

#include "make_message.hpp"

#include <iostream>

#include <functional>

namespace Apps{
namespace TCP_Client{

template<bool UseSSL>
class TCP_Client final :
		public My_Async::TCP::Client_Base<TCP_Client<UseSSL>, UseSSL, Byte_Array>,
		public std::enable_shared_from_this<TCP_Client<UseSSL>>
{
		using self_type = TCP_Client<UseSSL>;
		using base_type = My_Async::TCP::Client_Base<TCP_Client<UseSSL>, UseSSL, Byte_Array>;
	public:
		using base_type::Client_Base;

		std::function<void(void)> close_cb;
		std::function<void(void)> send_status;

		boost::asio::ip::tcp::endpoint local;
	protected:
		void read_handler(Byte_Array data) noexcept override
		{
			Core::Propagator::write_all(
							Byte_Array(
								make_received_message(
										this->local_endpoint(),
										data))
							);
		}

		void on_open() noexcept override
		{
			//Local endpoint must be saved to be removed (be finded) from container when closed.
			local = boost::asio::ip::tcp::endpoint{this->local_endpoint()};
		}

		void on_error(boost::system::error_code ec, char const* what) noexcept override
		{
			std::cerr << what << "[" << ec.value() << "]: " << ec.message() << "\n";
			Core::fail_message(Message::App::tcp_client, ec, what);
		}

		void on_close(boost::system::error_code) noexcept override
		{
			this->close_cb();
			this->send_status();
		}
};

}//TCP_Client
}//Apps


#endif /* TCP_CLIENT_APP_TYPE_DEF_HPP__ */
