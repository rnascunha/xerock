#include "share_data.hpp"
#include "make.hpp"

namespace Apps{
namespace TCP_Server{

void Data_Share::read_handler(Byte_Array data,
		boost::asio::ip::tcp::endpoint const& ep,
		boost::asio::ip::tcp::endpoint const& local_ep)
{
	Core::Propagator::write_all(
				Byte_Array(
					make_tcp_server_received_message(
							ep,
							local_ep,
							data)
				));
}

}//TCP_Server
}//Apps
