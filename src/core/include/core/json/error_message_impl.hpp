#ifndef MAKE_CORE_ERROR_MESSAGE_IMPL_HPP__
#define MAKE_CORE_ERROR_MESSAGE_IMPL_HPP__

#include "../error_message.hpp"
#include "byte_array.h"

template<typename ErrorType>
std::string make_error_message(Message::App app,
		ErrorType ec,
		const char* what)
{
	rapidjson::Document doc;

	doc.SetObject();

	doc.AddMember("code", ec.value(), doc.GetAllocator());
	doc.AddMember("message",
			rapidjson::Value(ec.message().c_str(), doc.GetAllocator()).Move(),
			doc.GetAllocator());
	doc.AddMember("what", rapidjson::Value(what, doc.GetAllocator()), doc.GetAllocator());

	return Message::Factory::create(app,
					Message::Control::error,
					doc);
}

#endif /* MAKE_CORE_ERROR_MESSAGE_IMPL_HPP__ */
