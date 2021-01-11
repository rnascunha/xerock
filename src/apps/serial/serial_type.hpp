#ifndef SERIAL_APP_TYPE_DEF_HPP__
#define SERIAL_APP_TYPE_DEF_HPP__

#include "my_async.hpp"
#include <memory>
#include <string>

namespace Apps{
namespace Serial{

class Serial final : public My_Async::Serial::Serial_Base<Serial, Byte_Array>,
				public std::enable_shared_from_this<Serial>
{
	using base_type = My_Async::Serial::Serial_Base<Serial, Byte_Array>;
	public:
		Serial(boost::asio::io_context& ioc);

		void set_close_callback(std::function<void()>);
	protected:
		void read_handler(Byte_Array data) noexcept override;
		void fail(boost::system::error_code ec, const char* what) noexcept override;

		std::function<void()> close_callback = nullptr;
};

}//Serial
}//Apps



#endif /* SERIAL_APP_TYPE_DEF_HPP__ */
