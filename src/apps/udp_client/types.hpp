#ifndef UDP_CLIENT_APP_TYPES_HPP__
#define UDP_CLIENT_APP_TYPES_HPP__

#include <cstdint>
#include <vector>
#include <boost/asio.hpp>

#include <byte_array.h>

#include "my_async.hpp"

#include "udp_client_type.hpp"

namespace Apps{
namespace UDP_Client{

using UDP_Container = My_Async::UDP::Simple_Container<UDP_Client>;

struct Socket_Info
{
	boost::asio::ip::udp::endpoint local;
	boost::asio::ip::udp::endpoint remote;
};

struct Message_Open
{
	boost::asio::ip::udp::endpoint remote;
};

struct Message_Status
{
	std::vector<Socket_Info> sockets;
};

struct Message_Close
{
	boost::asio::ip::udp::endpoint local;
};

struct Message_Data
{
	boost::asio::ip::udp::endpoint local;
	Byte_Array 	data;
};

}//UDP_Client
}//Apps

#endif /* UDP_CLIENT_APP_TYPES_HPP__ */
