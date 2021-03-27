#ifndef UDP_SERVER_APP_TYPE_DEF_HPP__
#define UDP_SERVER_APP_TYPE_DEF_HPP__

#include "boost/asio.hpp"

#include "my_async.hpp"
#include "byte_array.h"

namespace Apps{
namespace UDP_Server{

class UDP_Server final :
		public My_Async::UDP::Server_Base<UDP_Server, Byte_Array>,
		public std::enable_shared_from_this<UDP_Server>
{
		using self_type = UDP_Server;
		using base_type = My_Async::UDP::Server_Base<UDP_Server, Byte_Array>;
	public:
		using base_type::Server_Base;
	protected:
		void read_handler(Byte_Array data) noexcept override;
		void on_error(boost::system::error_code ec, char const* what) noexcept override;
};

}//UDP_Server
}//Apps

#endif /* UDP_SERVER_APP_TYPE_DEF_HPP__ */
