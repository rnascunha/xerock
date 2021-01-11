#ifndef MAKE_CORE_ERROR_MESSAGE_HPP__
#define MAKE_CORE_ERROR_MESSAGE_HPP__

#include "core/message.hpp"

template<typename ErrorType>
Message::Type_Send make_error_message(
		Message::App app,
		ErrorType ec,
		const char* what);

#endif /* MAKE_CORE_ERROR_MESSAGE_HPP__ */

#ifdef JSON_MESSAGES
#include "json/error_message_impl.hpp"
#else
#error Message type not defined
#endif
