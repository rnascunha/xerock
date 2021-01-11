#ifndef TCP_SERVER_APP_SHARE_DATA_HPP__
#define TCP_SERVER_APP_SHARE_DATA_HPP__

#include <cstdint>
#include <memory>

#include "byte_array.h"
#include "boost/asio.hpp"

namespace Apps{
namespace TCP_Server{

struct Data_Share
{
	bool keepalive = false;
	int32_t idle;
	int32_t interval;
	int32_t count;

	void read_handler(Byte_Array,
			boost::asio::ip::tcp::endpoint const& ep,
			boost::asio::ip::tcp::endpoint const& local_ep);

	std::function<void()> send_status = nullptr;
};

}//TCP_Server
}//Apps

#endif /* TCP_SERVER_APP_SHARE_DATA_HPP__ */
