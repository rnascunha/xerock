#include "../parser.hpp"
#include "defines.hpp"
#include "core/app.hpp"

#include <rapidjson/document.h>

namespace Apps{
namespace Main{

//Main
static bool check_message(const rapidjson::Document& doc);
static bool get_type(const rapidjson::Document& doc, Message::Type& type, Message::Control& control);
static bool get_app(const rapidjson::Document& doc, Message::App& app);
static uint64_t get_time(const rapidjson::Document& doc);

bool parser_main(const rapidjson::Document& doc, Message::Main_Data& data){
	if(!check_message(doc)) return false;
	if(!get_app(doc, data.app)) return false;
	if(!get_type(doc, data.type, data.control)) return false;
	data.time = get_time(doc);

	return true;
}

static uint64_t get_time(const rapidjson::Document& doc){
	return doc[MESSAGE_TIME_KEY].GetUint64();
}

static bool get_type(const rapidjson::Document& doc, Message::Type& type, Message::Control& control){
	if(std::string(doc[MESSAGE_TYPE_KEY].GetString()) == MESSAGE_TYPE_DATA){
		type = Message::Type::data;
		return true;
	}

	if(std::string(doc[MESSAGE_TYPE_KEY].GetString()) != MESSAGE_TYPE_CONTROL){
		return false;
	}
	type = Message::Type::control;

	if(!(doc.HasMember(CONTROL_TYPE_KEY) && doc[CONTROL_TYPE_KEY].IsString())){
		return false;
	}

	if(std::string(doc[CONTROL_TYPE_KEY].GetString()) == CONTROL_TYPE_STATUS)
		control = Message::Control::status;
	else if(std::string(doc[CONTROL_TYPE_KEY].GetString()) == CONTROL_TYPE_ERROR)
		control = Message::Control::error;
	else if(std::string(doc[CONTROL_TYPE_KEY].GetString()) == CONTROL_TYPE_OPEN)
		control = Message::Control::open;
	else if(std::string(doc[CONTROL_TYPE_KEY].GetString()) == CONTROL_TYPE_CLOSE)
		control = Message::Control::close;
	else if(std::string(doc[CONTROL_TYPE_KEY].GetString()) == CONTROL_TYPE_CONFIG)
		control = Message::Control::config;
	else if(std::string(doc[CONTROL_TYPE_KEY].GetString()) == CONTROL_TYPE_CUSTOM)
		control = Message::Control::custom;
	else
		return false;

	return true;
}

static bool get_app(const rapidjson::Document& doc, Message::App& app){
	Core::App a;
	if(!Core::Dispatcher::name(a, doc[MESSAGE_APP_KEY].GetString()))
		return false;

	app = a.app();
	return true;
}

static bool check_message(const rapidjson::Document& doc){
	return doc.IsObject() &&
			(doc.HasMember(MESSAGE_APP_KEY) && doc[MESSAGE_APP_KEY].IsString()) &&
			(doc.HasMember(MESSAGE_TIME_KEY) && doc[MESSAGE_TIME_KEY].IsUint64()) &&
			(doc.HasMember(MESSAGE_TYPE_KEY) && doc[MESSAGE_TYPE_KEY].IsString());
}

}//Main
}//Apps


