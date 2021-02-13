#include "udp_server_type.hpp"
#include "core/helpers.hpp"
#include "make.hpp"

namespace Apps{
namespace UDP_Server{

void
UDP_Server::
read_handler(Byte_Array data) noexcept
{
	Core::Propagator::write_all(
		Byte_Array(
			make_received_message(
				this->local_endpoint(),
				this->endpoint(),
				data
			)
		)
	);
}

void
UDP_Server::
on_error(boost::system::error_code ec, char const* what) noexcept
{
	std::cerr << what << ": " << ec.message() << "\n";
	Core::fail_message(Message::App::tcp_server, ec, what);
}

}//UDP_Server
}//Apps
