#ifndef UDP_CLIENT_JSON_MAKE_MESSAGE2_HPP__
#define UDP_CLIENT_JSON_MAKE_MESSAGE2_HPP__

#include "core/message.hpp"
#include "byte_array.h"
#include <memory>
//#include "my_async.hpp"

namespace Apps{
namespace UDP_Client{

//Message::Type_Send make_status_message(std::shared_ptr<TCP_Container>);
Message::Type_Send make_received_message(
		boost::asio::ip::udp::endpoint const&,
		Byte_Array const&);

}//UDP_Client
}//Apps

#endif /* UDP_CLIENT_JSON_MAKE_MESSAGE2_HPP__ */
