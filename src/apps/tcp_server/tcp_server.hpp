#ifndef TCP_SERVER_APP_HPP__
#define TCP_SERVER_APP_HPP__

#include "core/message.hpp"
#include "types.hpp"
#include <memory>

namespace Apps{
namespace TCP_Server{

static constexpr const char* name = "tcp_server";

void message_status(std::shared_ptr<TCP_Container>);
void tpc_server_app(std::shared_ptr<Core::Propagator>,
		Message::Main_Data const&, Message::Type_Recv const&, std::shared_ptr<TCP_Container>);
void init_cb(std::shared_ptr<TCP_Container>);

}//TCP_Server
}//Apps

#endif /* TCP_SERVER_APP_HPP__ */
