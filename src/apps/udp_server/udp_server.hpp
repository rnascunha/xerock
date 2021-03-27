#ifndef UDP_SERVER_APP_HPP__
#define UDP_SERVER_APP_HPP__

#include "core/message.hpp"
#include "types.hpp"
#include <memory>

namespace Apps{
namespace UDP_Server{

static constexpr const char* name = "udp_server";

void udp_server_app(std::shared_ptr<Core::Propagator>,
		Message::Main_Data const&,
		Message::Type_Recv const&,
		std::shared_ptr<Container>);

}//UDP_Server
}//Apps

#endif /* UDP_SERVER_APP_HPP__ */
