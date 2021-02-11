#ifndef UDP_CLIENT_APP_TYPE_DEF_HPP__
#define UDP_CLIENT_APP_TYPE_DEF_HPP__

#include "boost/asio.hpp"

#include "core/helpers.hpp"
#include "my_async.hpp"
#include "byte_array.h"

#include "make_message.hpp"

#include <iostream>

#include <functional>

namespace Apps{
namespace UDP_Client{

class UDP_Client final :
		public My_Async::UDP::Client_Base<UDP_Client, Byte_Array>,
		public std::enable_shared_from_this<UDP_Client>
{
		using self_type = UDP_Client;
		using base_type = My_Async::UDP::Client_Base<UDP_Client, Byte_Array>;
	public:
		using base_type::Client_Base;

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

		void on_error(boost::system::error_code ec, char const* what) noexcept override
		{
			std::cerr << what << "[" << ec.value() << "]: " << ec.message() << "\n";
			Core::fail_message(Message::App::udp_client, ec, what);
		}
};

}//UDP_Client
}//Apps


#endif /* UDP_CLIENT_APP_TYPE_DEF_HPP__ */
