#ifndef MAIN_APP_MAKE_MESSAGE_HPP__
#define MAIN_APP_MAKE_MESSAGE_HPP__

#include "core/message.hpp"

namespace Apps{
namespace Main{

Message::Type_Send make_status(unsigned int nusers);
Message::Type_Send make_uid();
Message::Type_Send make_app_list(std::vector<std::string> const& app_list);

}//Main
}//Apps

#endif /* MAIN_APP_MAKE_MESSAGE_HPP__ */
