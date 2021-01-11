#ifndef MESSAGE_FACTORY_JSON_HPP__
#define MESSAGE_FACTORY_JSON_HPP__

#include <string>

#include "../message.hpp"
#include "byte_array.h"

#include "defines.hpp"
#include "../functions.hpp"

#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/document.h"

namespace Message{

class Factory{
	public:
		//Data
		static std::string create(App, int);
		static std::string create(App, unsigned);
		static std::string create(App, std::string const&);
		static std::string create(App, Byte_Array const&);
//		static std::string create(App, rapidjson::Document&);
		static std::string create(App, rapidjson::Value&);
		static std::string create(App, std::vector<std::string> const&);

		//Control
		static std::string create(App, Control);
		static std::string create(App, Control, int);
		static std::string create(App, Control, unsigned);
		static std::string create(App, Control, std::string const&);
		static std::string create(App, Control, Byte_Array const&);
		static std::string create(App, Control, rapidjson::Document&);
		static std::string create(App, Control, std::vector<std::string> const&);

		template<bool update_time = false>
		static std::string add_mid(rapidjson::Document& doc);

};//Factory

using JSON_Writer = rapidjson::Writer<rapidjson::StringBuffer>;

template<bool update_time>
std::string
Factory::add_mid(rapidjson::Document& doc)
{
	doc.AddMember(MESSAGE_MID_KEY, Core::get_mid(), doc.GetAllocator());
	doc.AddMember(MESSAGE_ECHOED_KEY, true, doc.GetAllocator());

	rapidjson::StringBuffer s;
	JSON_Writer writer(s);

	if constexpr(update_time)
	{
		doc[MESSAGE_TIME_KEY] = time();
	}

	doc.Accept(writer);

	return std::string(s.GetString());
}

}//Message

#endif /* MESSAGE_FACTORY_JSON_HPP__ */
