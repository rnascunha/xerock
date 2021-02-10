#ifndef UDP_CLIENT_APP_HPP__
#define UDP_CLIENT_APP_HPP__

#include "core/message.hpp"
#include "types.hpp"
#include <memory>

namespace Apps{
namespace UDP_Client{

static constexpr const char* name = "udp_client";

void udp_client_app(std::shared_ptr<Core::Propagator>,
		Message::Main_Data const&,
		Message::Type_Recv const&,
		std::shared_ptr<UDP_Container>);

}//UDP_Client
}//Apps

#endif /* UDP_CLIENT_APP_HPP__ */
