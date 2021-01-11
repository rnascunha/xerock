#ifndef SERIAL_APP_MAKE_MESSAGE_HPP__
#define SERIAL_APP_MAKE_MESSAGE_HPP__

#include <string>
#include "byte_array.h"
#include <memory>
#include "core/types.hpp"
#include "container.hpp"

namespace Apps{
namespace Serial{

void serial_send_status(std::shared_ptr<Container>);
void serial_send_status(std::shared_ptr<Core::Propagator>, std::shared_ptr<Container>);
Byte_Array serial_make_data(std::string const& str, Byte_Array const& data);

}//Serial
}//Apps

#endif /* SERIAL_APP_MAKE_MESSAGE_HPP__ */
