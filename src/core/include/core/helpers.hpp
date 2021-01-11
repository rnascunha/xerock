#ifndef APPS_FUNCTION_HELPER_HPP__
#define APPS_FUNCTION_HELPER_HPP__

#include "types.hpp"
#include "message.hpp"

namespace Core{

template<typename ErrorType>
void fail_message(std::shared_ptr<Propagator>,
		Message::App,
		ErrorType,
		const char* = "") noexcept;

template<typename ErrorType>
void fail_message(Message::App,
		ErrorType,
		const char* = "") noexcept;

}//Core

#include "json/fail_message_impl.hpp"

#endif /* APPS_FUNCTION_HELPER_HPP__ */
