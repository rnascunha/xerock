#ifndef SERIAL_UTIL_ASYNC_HPP__
#define SERIAL_UTIL_ASYNC_HPP__

#include <vector>
#include <string>
#include <initializer_list>

#include "boost/asio.hpp"

namespace My_Async{
namespace Serial{

std::vector<std::string> scan_ports();

template<typename Serial_Type, typename ...Args>
std::shared_ptr<Serial_Type> make_serial(
		boost::asio::io_context& ioc, Args ...args) noexcept
{
	return std::make_shared<Serial_Type>(ioc, args...);
}

}//Serial
}//My_Async

#endif /* SERIAL_UTIL_ASYNC_HPP__ */
