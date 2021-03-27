#ifndef UDP_SERVER_MAKE_MESSAGE_HPP__
#define UDP_SERVER_MAKE_MESSAGE_HPP__

#include "core/message.hpp"
#include "types.hpp"
#include "my_async.hpp"

namespace Apps{
namespace UDP_Server{

Message::Type_Send make_status_message(std::shared_ptr<Container>);
Message::Type_Send make_received_message(
		boost::asio::ip::udp::endpoint const& to,
		boost::asio::ip::udp::endpoint const& from,
		Byte_Array const&);

}//UDP_Server
}//Apps


#endif /* UDP_SERVER_MAKE_MESSAGE_HPP__ */
