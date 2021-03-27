#ifndef UDP_SERVER_APP_TYPES_HPP__
#define UDP_SERVER_APP_TYPES_HPP__

#include <cstdint>
#include <vector>
#include <boost/asio.hpp>

#include <byte_array.h>

#include "my_async.hpp"

#include "udp_server_type.hpp"

namespace Apps{
namespace UDP_Server{

using Container = My_Async::UDP::Simple_Container_Server<UDP_Server>;

struct Message_Status
{
	std::vector<boost::asio::ip::udp::endpoint> eps;
};

struct Message_Open
{
	boost::asio::ip::udp::endpoint ep;
};

struct Message_Close
{
	boost::asio::ip::udp::endpoint ep;
};

struct Message_Data
{
	boost::asio::ip::udp::endpoint from;
	std::vector<boost::asio::ip::udp::endpoint> to;
	Byte_Array data;
};

}//UDP_Server
}//Apps

#endif /* UDP_SERVER_APP_TYPES_HPP__ */
