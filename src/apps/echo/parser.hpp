#ifndef ECHO_APP_JSON_PARSER_HPP__
#define ECHO_APP_JSON_PARSER_HPP__

#include "types.hpp"
#include "core/message.hpp"

namespace Apps{
namespace Echo{

bool parser_echo(const Message::Type_Recv&, Echo_Data&);

}//Echo
}//Apps

#endif /* ECHO_APP_JSON_PARSER_HPP__ */
