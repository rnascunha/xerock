#ifndef TCP_CLIENT_APP_TYPES_HPP__
#define TCP_CLIENT_APP_TYPES_HPP__

#include <cstdint>
#include <vector>
#include <boost/asio.hpp>

#include <byte_array.h>

#include "my_async.hpp"

#include "tcp_client_type.hpp"

namespace Apps{
namespace TCP_Client{

template<bool UseSSL>
using Client = Apps::TCP_Client::TCP_Client<UseSSL>;

#if USE_APP_TCP_CLIENT == 1
using TCP_Container = My_Async::TCP::Container<false, true>;	//Plain
#elif USE_APP_TCP_CLIENT == 2
using TCP_Container = My_Async::TCP::Container<true, false>;	//SSL
#elif USE_APP_TCP_CLIENT == 3
using TCP_Container = My_Async::TCP::Container<true, true>;		//Plain e SSL
#else
#error "No proper compile option made"
#endif /* USE_APP_TCP_CLIENT == 1 */

enum class Secure_Type
{
	plain = 0,
	ssl
};

struct Socket_Info
{
	Secure_Type secure;
	boost::asio::ip::tcp::endpoint local;
	boost::asio::ip::tcp::endpoint remote;
	struct
	{
		bool keep_alive = false;
		int32_t idle;
		int32_t interval;
		int32_t count;
	} options;
};

struct Message_Open
{
	Secure_Type secure;
	boost::asio::ip::tcp::endpoint remote;
	struct
	{
		bool keep_alive = false;
		int32_t idle;
		int32_t interval;
		int32_t count;
	} options;
};

struct Message_Status
{
	std::vector<Socket_Info> sockets;
};

struct Message_Close{
	boost::asio::ip::tcp::endpoint local;
};

struct Message_Data
{
	boost::asio::ip::tcp::endpoint local;
	Byte_Array 	data;
};

}//TCP_Client
}//Apps

#endif /* TCP_CLIENT_APP_TYPES_HPP__ */
