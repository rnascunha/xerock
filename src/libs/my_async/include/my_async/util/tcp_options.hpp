#ifndef MY_ASYNC_TCP_OPTIONS_HPP__
#define MY_ASYNC_TCP_OPTIONS_HPP__

namespace My_Async{
namespace TCP{

template<typename Socket>
void keepalive(Socket const& socket, int32_t idle, int32_t count, int32_t interval) noexcept;

}//TCP
}//My_Async

#include "impl/tcp_options_impl.hpp"

#endif /* MY_ASYNC_TCP_OPTIONS_HPP__ */
