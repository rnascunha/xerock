#ifndef TCP_SERVER_APP_PARSER_HPP__
#define TCP_SERVER_APP_PARSER_HPP__

#include "core/message.hpp"
#include "types.hpp"

#include "byte_array.h"

namespace Apps{
namespace TCP_Server{

bool parser_data(const Message::Type_Recv&, Message_Data&);
bool parser_open(const Message::Type_Recv&, Message_Open&);
bool parser_close(const Message::Type_Recv&, Message_Close&);
bool parser_close_clients(const Message::Type_Recv&, Message_Close_Client&);

}//TCP_Server
}//Apps

#endif /* TCP_SERVER_APP_PARSER_HPP__ */
