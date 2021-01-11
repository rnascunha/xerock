#ifndef TCP_SERVER_APP_TYPES_HPP__
#define TCP_SERVER_APP_TYPES_HPP__

#include <cstdint>
#include <vector>
#include <boost/asio.hpp>

#include <byte_array.h>

#include "my_async.hpp"

#include "tcp_server_type.hpp"

namespace Apps{
namespace TCP_Server{

using client_list = std::vector<boost::asio::ip::tcp::endpoint>;

template<bool UseSSL>
using Session = Apps::TCP_Server::Session_TCP_Server<UseSSL>;
template<bool UseSSL>
using Listener = My_Async::Listener::Listener_Base<Session<UseSSL>>;

#if USE_APP_TCP_SERVER == 1
using TCP_Container = My_Async::Listener::Container<false, true>;	//Plain
#elif USE_APP_TCP_SERVER == 2
using TCP_Container = My_Async::Listener::Container<true, false>;	//SSL
#elif USE_APP_TCP_SERVER == 3
using TCP_Container = My_Async::Listener::Container<true, true>;	//Plain e SSL
#else
#error "No proper compile option made"
#endif /* USE_APP_TCP_SERVER == 1 */

enum class Secure_Type
{
	plain = 0,
	ssl
};

enum class Custom_Type
{
	close_client
};

struct Message_Status
{
	uint16_t 	port;
	client_list	clients;
};

struct Message_Open
{
	Secure_Type secure;
	uint16_t port;
	std::string addr;
	struct
	{
		bool keep_alive = false;
		int32_t idle;
		int32_t interval;
		int32_t count;
	} options;
};

struct Message_Close{
	boost::asio::ip::tcp::endpoint endpoint;
};

struct Message_Close_Client
{
	boost::asio::ip::tcp::endpoint endpoint;
	client_list clients;
};

struct Message_Data
{
	boost::asio::ip::tcp::endpoint from;
	client_list to;
	Byte_Array 	data;
};

}//TCP_Server
}//Apps

#endif /* TCP_SERVER_APP_TYPES_HPP__ */
