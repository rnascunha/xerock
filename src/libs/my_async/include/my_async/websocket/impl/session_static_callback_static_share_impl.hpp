#ifndef WEBSOCKET_SESSION_STATIC_CALLBACK_STATIC_SHARE_ASYNC_IMPL_HPP__
#define WEBSOCKET_SESSION_STATIC_CALLBACK_STATIC_SHARE_ASYNC_IMPL_HPP__

#include "../session_static_callback_static_share.hpp"

namespace My_Async{
namespace Websocket{

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
typename Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_open_t Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_open_f;

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
typename Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_close_t Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_close_f;

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
typename Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_message_t Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_message_f;

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
typename Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_error_t Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_error_f;

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
~Session_Static_Callback_Static_Share()
{
	shared_type::leave(this);
}

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
write_all(InContainer const data)
{
#ifdef _MSC_VER
	/*
	* MSVC you can't bind bound functions. You need to wrap it first
	*/
	std::function<void(self_type*)> f = std::bind(
		&self_type::write_share,
		std::placeholders::_1,
		std::make_shared<InContainer const>(std::move(data))
	);
	shared_type::exec_to_all(f);
#else
	shared_type::exec_to_all(
		std::bind(
			&self_type::write_share,
			std::placeholders::_1,
			std::make_shared<InContainer const>(std::move(data))
		)
	);
#endif
}

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
write_all(InContainer const data, bool text)
{	
#ifdef _MSC_VER
	/*
	* MSVC you can't bind bound functions. You need to wrap it first
	*/
	std::function<void(self_type*)> f = std::bind(
		&self_type::write_share2,
		std::placeholders::_1,
		std::make_shared<InContainer const>(std::move(data)),
		text
	);
	shared_type::exec_to_all(f);
#else
	shared_type::exec_to_all(
		std::bind(
			&self_type::write_share2,
			std::placeholders::_1,
			std::make_shared<InContainer const>(std::move(data)),
			text
		)
	);
#endif
}

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
set_on_open(on_open_t func /* = nullptr */) noexcept
{
	on_open_f = func;
}

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
set_on_close(on_close_t func /* = nullptr */) noexcept
{
	on_close_f = func;
}

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
set_on_message(on_message_t func /* = nullptr */) noexcept{
	on_message_f = func;
}

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
set_on_error(on_error_t func /* = nullptr */) noexcept
{
	on_error_f = func;
}

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_open() noexcept
{
	shared_type::join(this);
	if(on_open_f) on_open_f(this->shared_from_this());
}

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
read_handler(InContainer data) noexcept
{
	if(on_message_f) on_message_f(this->shared_from_this(), data);
}

template<bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>::
fail(boost::system::error_code ec, char const* what) noexcept
{
	if(ec == boost::asio::error::operation_aborted ||
	   ec == boost::beast::websocket::error::closed)
	{
		if(on_close_f) on_close_f(this->shared_from_this(), ec);
		return;
	}

	if(on_error_f) on_error_f(this->shared_from_this(), ec, what);
}

}//Websocket
}//My_Async


#endif /* WEBSOCKET_SESSION_STATIC_CALLBACK_STATIC_SHARE_ASYNC_IMPL_HPP__ */
