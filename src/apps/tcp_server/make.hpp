#ifndef TCP_SERVER_JSON_MAKE_MESSAGE_HPP__
#define TCP_SERVER_JSON_MAKE_MESSAGE_HPP__

#include "core/message.hpp"
#include "types.hpp"
#include "my_async.hpp"

namespace Apps{
namespace TCP_Server{

Message::Type_Send make_status_message(std::shared_ptr<TCP_Container>);
Message::Type_Send make_tcp_server_received_message(
		boost::asio::ip::tcp::endpoint const&,
		boost::asio::ip::tcp::endpoint const&,
		Byte_Array const&);

}//TCP_Server
}//Apps


#endif /* TCP_SERVER_JSON_MAKE_MESSAGE_HPP__ */
