#ifndef TCP_CLIENT_JSON_MAKE_MESSAGE2_HPP__
#define TCP_CLIENT_JSON_MAKE_MESSAGE2_HPP__

#include "core/message.hpp"
#include "byte_array.h"
#include <memory>
//#include "my_async.hpp"

namespace Apps{
namespace TCP_Client{

//Message::Type_Send make_status_message(std::shared_ptr<TCP_Container>);
Message::Type_Send make_received_message(
		boost::asio::ip::tcp::endpoint const&,
		Byte_Array const&);

}//TCP_Client
}//Apps

#endif /* TCP_CLIENT_JSON_MAKE_MESSAGE2_HPP__ */
