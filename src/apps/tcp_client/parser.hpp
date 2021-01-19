#ifndef TCP_CLIENT_APP_PARSER_HPP__
#define TCP_CLIENT_APP_PARSER_HPP__

#include "core/message.hpp"
#include "types.hpp"

namespace Apps{
namespace TCP_Client{

bool parser_data(const Message::Type_Recv&, Message_Data&);
bool parser_open(const Message::Type_Recv&, Message_Open&);
bool parser_close(const Message::Type_Recv&, Message_Close&);

}//TCP_Client
}//Apps

#endif /* TCP_CLIENT_APP_PARSER_HPP__ */
