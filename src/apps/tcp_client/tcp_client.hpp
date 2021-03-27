#ifndef TCP_CLIENT_APP_HPP__
#define TCP_CLIENT_APP_HPP__

#include "core/message.hpp"
#include "types.hpp"
#include <memory>

namespace Apps{
namespace TCP_Client{

static constexpr const char* name = "tcp_client";

void message_status(std::shared_ptr<TCP_Container>);
void tpc_client_app(std::shared_ptr<Core::Propagator>,
		Message::Main_Data const&,
		Message::Type_Recv const&,
		std::shared_ptr<TCP_Container>);

}//TCP_Client
}//Apps

#endif /* TCP_CLIENT_APP_HPP__ */
