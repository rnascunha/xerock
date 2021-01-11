#ifndef APP_SERIAL_CONTAINER_HPP__
#define APP_SERIAL_CONTAINER_HPP__

#include "boost/asio.hpp"
#include "my_async.hpp"
#include "byte_array.h"
#include <memory>

#include "serial_type.hpp"

namespace Apps{
namespace Serial{

using Container = My_Async::Serial::Container<Serial>;

}//Serial
}//Apps


#endif /* APP_SERIAL_CONTAINER_HPP__ */
