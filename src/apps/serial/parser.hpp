#ifndef SERIAL_APP_JSON_PARSER_HPP__
#define SERIAL_APP_JSON_PARSER_HPP__

#include "core/message.hpp"
#include "types.hpp"

#include "byte_array.h"

namespace Apps{
namespace Serial{

bool parser_serial_config(const Message::Type_Recv& doc, Serial_Config& data);
bool parser_serial_close(const Message::Type_Recv&, std::string&);
bool parser_serial_data(const Message::Type_Recv&, std::string&, Byte_Array&);

}//Serial
}//Apps

#endif /* SERIAL_APP_JSON_PARSER_HPP__ */
