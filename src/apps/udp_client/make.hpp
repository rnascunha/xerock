#ifndef UDP_CLIENT_JSON_MAKE_MESSAGE_HPP__
#define UDP_CLIENT_JSON_MAKE_MESSAGE_HPP__

#include "core/message.hpp"
#include "types.hpp"
#include <memory>

namespace Apps{
namespace UDP_Client{

Message::Type_Send make_status_message(std::shared_ptr<UDP_Container>);
//Message::Type_Send make_received_message(
//		boost::asio::ip::tcp::endpoint const&,
//		Byte_Array const&);

}//UDP_Client
}//Apps

#endif /* UDP_CLIENT_JSON_MAKE_MESSAGE_HPP__ */
