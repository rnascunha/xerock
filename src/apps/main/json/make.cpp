#include "../make.hpp"
#include <string>
#include "core/json/factory.hpp"
#include "defines.hpp"

namespace Apps{
namespace Main{

Message::Type_Send make_status(unsigned int nusers)
{
	rapidjson::Document doc;

	doc.SetObject();
	doc.AddMember(CONTROL_TYPE_STATUS_NUSERS, nusers, doc.GetAllocator());

	auto msg = Message::Factory::create(Message::App::main,
			Message::Control::status,
			doc);

	return msg;
}

Message::Type_Send make_uid()
{
	rapidjson::Document doc;

	doc.SetObject();
	doc.AddMember(CONTROL_TYPE_STATUS_UID, Core::user_id(), doc.GetAllocator());

	auto msg = Message::Factory::create(Message::App::main,
			Message::Control::status,
			doc);

	return msg;
}

Message::Type_Send make_app_list(std::vector<std::string> const& app_list)
{
	return Message::Factory::create(Message::App::main,
				Message::Control::config,
				app_list);
}

}//Main
}//Apps

