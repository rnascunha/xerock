#ifndef MAIN_APP_JSON_PARSER_HPP__
#define MAIN_APP_JSON_PARSER_HPP__

#include "core/message.hpp"

namespace Apps{
namespace Main{

bool parser_main(const Message::Type_Recv& doc, Message::Main_Data& data);

}//Main
}//Apps

#endif /* MAIN_APP_JSON_PARSER_HPP__ */
