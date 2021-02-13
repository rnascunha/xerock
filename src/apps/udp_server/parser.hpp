#ifndef UDP_SERVER_APP_PARSER_HPP__
#define UDP_SERVER_APP_PARSER_HPP__

#include "core/message.hpp"
#include "types.hpp"

namespace Apps{
namespace UDP_Server{

bool parser_data(const Message::Type_Recv&, Message_Data&);
bool parser_open(const Message::Type_Recv&, Message_Open&);
bool parser_close(const Message::Type_Recv&, Message_Close&);

}//UDP_Server
}//Apps

#endif /* UDP_SERVER_APP_PARSER_HPP__ */
