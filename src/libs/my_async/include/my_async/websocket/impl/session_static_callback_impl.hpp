#ifndef WEBSOCKET_SESSION_STATIC_CALLBACK_ASYNC_IMPL_HPP__
#define WEBSOCKET_SESSION_STATIC_CALLBACK_ASYNC_IMPL_HPP__

#include "../session_static_callback.hpp"

namespace My_Async{
namespace Websocket{

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
typename Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_open_t Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_open_f;

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
typename Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_close_t Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_close_f;

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
typename Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_message_t Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_message_f;

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
typename Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_error_t Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_error_f;

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>::
set_on_open(on_open_t func /* = nullptr */) noexcept
{
	on_open_f = func;
}

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>::
set_on_close(on_close_t func /* = nullptr */) noexcept
{
	on_close_f = func;
}

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>::
set_on_message(on_message_t func /* = nullptr */) noexcept
{
	on_message_f = func;
}

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>::
set_on_error(on_error_t func /* = nullptr */) noexcept
{
	on_error_f = func;
}

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_open(boost::system::error_code ec) noexcept
{
	if(on_open_f) on_open_f(this->shared_from_this());
}

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>::
read_handler(InContainer data) noexcept
{
	if(on_message_f) on_message_f(this->shared_from_this(), data);
}

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>::
fail(boost::system::error_code ec, char const* what) noexcept
{
	if(ec == boost::asio::error::operation_aborted ||
	   ec == boost::beast::websocket::error::closed){
		if(on_close_f) on_close_f(this->shared_from_this(), ec);
		return;
	}

	if(on_error_f) on_error_f(this->shared_from_this(), ec, what);
}

}//Websocket
}//My_Async




#endif /* WEBSOCKET_SESSION_STATIC_CALLBACK_ASYNC_IMPL_HPP__ */

