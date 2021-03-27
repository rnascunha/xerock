#include <memory>
#include "../types.hpp"

#include "byte_array.h"

#include "../make.hpp"

#include "core/helpers.hpp"


namespace Apps{
namespace TCP_Server{

template<bool UseSSL>
void on_error(std::shared_ptr<Session<UseSSL>>, boost::system::error_code ec, char const* what)
{
	std::cerr << what << ": " << ec.message() << "\n";
	Core::fail_message(Message::App::tcp_server, ec, what);
}

template<bool UseSSL>
static void on_close(std::shared_ptr<Session<UseSSL>>,
		boost::system::error_code, std::shared_ptr<TCP_Container> servers)
{
	message_status(servers);
}

template<bool UseSSL>
void on_open(std::shared_ptr<Session<UseSSL>> socket,
		std::shared_ptr<TCP_Container> servers)
{
	socket->keep_alive(1, 2, 1);
	message_status(servers);
}

template<bool UseSSL>
void on_message(std::shared_ptr<Session<UseSSL>> sock, Byte_Array data){
	Core::Propagator::write_all(
			Byte_Array(
				make_tcp_server_received_message(
						sock->endpoint(),
						sock->local_endpoint(),
						data)
			),
			true);
}

template<bool UseSSL>
void init_cb2(std::shared_ptr<TCP_Container> servers){
	Session<UseSSL>::set_on_open(std::bind(on_open<UseSSL>,
									std::placeholders::_1,
									servers));
	Session<UseSSL>::set_on_error(on_error<UseSSL>);
	Session<UseSSL>::set_on_message(on_message<UseSSL>);
	Session<UseSSL>::set_on_close(std::bind(on_close<UseSSL>,
									std::placeholders::_1,
									std::placeholders::_2,
									servers));
}

}//TCP_Server
}//Apps
