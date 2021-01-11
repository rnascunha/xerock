#ifndef SERIAL_ASYNC_STATIC_CALLBACK_IMPL_HPP__
#define SERIAL_ASYNC_STATIC_CALLBACK_IMPL_HPP__

#include <functional>
#include "byte_array.h"

namespace My_Async{
namespace Serial{

template<typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
typename Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>::
on_open_t Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>::
on_open_f;

template<typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
typename Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>::
on_close_t Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>::
on_close_f;

template<typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
typename Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>::
on_message_t Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>::
on_message_f;

template<typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
typename Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>::
on_error_t Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>::
on_error_f;

template<typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>::
on_open() noexcept
{
	if(on_open_f) on_open_f(this->shared_from_this());
}

template<typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>::
on_close(boost::system::error_code ec) noexcept
{
	if(on_close_f) on_close_f(this->shared_from_this(), ec);
}

template<typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>
::read_handler(InContainer data) noexcept
{
	if(on_message_f) on_message_f(this->shared_from_this(), data);
}

template<typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>
::fail(boost::system::error_code ec, const char* what) noexcept
{
	if(on_error_f) on_error_f(this->shared_from_this(), ec, what);
}

template<typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>
::set_on_open(on_open_t func /* = nullptr */) noexcept
{
	on_open_f = func;
}

template<typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>
::set_on_close(on_close_t func /* = nullptr */) noexcept
{
	on_close_f = func;
}

template<typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>
::set_on_message(on_message_t func /* = nullptr */) noexcept
{
	on_message_f = func;
}

template<typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>
::set_on_error(on_error_t func /* = nullptr */) noexcept
{
	on_error_f = func;
}

}//Serial
}//My_Async

#endif /* SERIAL_ASYNC_STATIC_CALLBACK_IMPL_HPP__ */
