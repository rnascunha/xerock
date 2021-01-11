#ifndef MESSAGE_JSON_SERIAL_APP_HPP__
#define MESSAGE_JSON_SERIAL_APP_HPP__

#include "core/message.hpp"
#include "byte_array.h"
#include "types.hpp"

#include "container.hpp"

namespace Apps{
namespace Serial{

static constexpr const char* name = "serial";

void serial_app(std::shared_ptr<Core::Propagator> ws,
		Message::Main_Data&, Message::Type_Recv&,
		std::shared_ptr<Container>);

}//Serial
}//Apps

#endif /* MESSAGE_JSON_SERIAL_APP_HPP__ */
