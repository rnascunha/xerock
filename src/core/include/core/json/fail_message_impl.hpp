#ifndef MAIN_APP_FAIL_MESSAGE_IMPL_HPP__
#define MAIN_APP_FAIL_MESSAGE_IMPL_HPP__


#include "../helpers.hpp"

#include <memory>

//#include "../websocket_session.hpp"
//#include "../types.hpp"

#include <system_error>
#include <rapidjson/document.h>
#include "byte_array.h"
#include "factory.hpp"
#include "../error_message.hpp"

namespace Core{

template<typename ErrorType>
void fail_message(std::shared_ptr<Core::Propagator> ws,
					Message::App app,
					ErrorType ec,
					const char* what /* = "" */) noexcept
{
	ws->write(Byte_Array(make_error_message(app, ec, what)), true);
}

template<typename ErrorType>
void fail_message(Message::App app,
		ErrorType ec,
		const char* what /* = "" */) noexcept
{
	Core::Propagator::write_all(
			Byte_Array(make_error_message(app, ec, what)), true);
}

}//Core

#endif /* MAIN_APP_FAIL_MESSAGE_IMPL_HPP__ */
