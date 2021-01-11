#include "serial_type.hpp"
#include "make.hpp"

namespace Apps{
namespace Serial{

Serial::
Serial(boost::asio::io_context& ioc)
: base_type::Serial_Base(ioc)
{}

void Serial::read_handler(Byte_Array data) noexcept
{
	auto data_send = serial_make_data(this->get_port_name(), data);
	Core::Propagator::write_all(data_send, true);
}

void Serial::set_close_callback(std::function<void()> f)
{
	close_callback = f;
}

void Serial::fail(boost::system::error_code ec, const char* what) noexcept
{
	if(ec.value() == boost::system::errc::no_such_file_or_directory)
		close_callback();
	else
		std::cerr << what << ": " << ec.value() << " " << ec.message() << "\n";
}

}//Serial
}//Apps
