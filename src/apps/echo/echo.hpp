#ifndef MESSAGE_ECHO_APP_HPP__
#define MESSAGE_ECHO_APP_HPP__

#include <memory>

#include "core/message.hpp"
#include "core/types.hpp"

namespace Apps{
namespace Echo{

static constexpr const char* name = "echo";

void echo_app(std::shared_ptr<Core::Propagator>,
		Message::Main_Data&,
		Message::Type_Recv&);

}//Echo
}//Apps

#endif /* MESSAGE_ECHO_APP_HPP__ */
