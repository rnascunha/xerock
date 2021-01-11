#ifndef MONITOR_APP_PARSER_HPP__
#define MONITOR_APP_PARSER_HPP__

#include "core/message.hpp"
#include "types.hpp"

namespace Apps{
namespace Monitor{

bool parse_app(const Message::Type_Recv& doc, Message_Monitor_Data& freq);

}//Monitor
}//Apps

#endif /* MONITOR_APP_PARSER_HPP__ */
