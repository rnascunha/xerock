#include "core/json/factory.hpp"
#include "core/app.hpp"

namespace Message{

static const char* app(App a);
static const char* type(Type t);
static const char* control(Control c);
static void make_data_obj(
		JSON_Writer& writer,
		App ap);
static void make_control_obj(
		JSON_Writer& writer,
		App ap,
		Control con);
static void make_data_array(JSON_Writer& writer,
		Byte_Array const& b);
//template<template<class> class Container_s>
//static void make_string_array(JSON_Writer& writer,
//		Container_s<std::string> const& value);
static void make_string_array(JSON_Writer& writer,
	std::vector<std::string> const& value);

//Data
std::string
Factory::create(App ap, int value){
	rapidjson::StringBuffer s;
	JSON_Writer writer(s);

	make_data_obj(writer, ap);

	writer.Key(DATA_KEY);
	writer.Int(value);

	writer.EndObject();

	return std::string(s.GetString());
}

std::string
Factory::create(App ap, unsigned value){
	rapidjson::StringBuffer s;
	JSON_Writer writer(s);

	make_data_obj(writer, ap);

	writer.Key(DATA_KEY);
	writer.Uint(value);

	writer.EndObject();

	return std::move(std::string(s.GetString()));
}

std::string
Factory::create(App ap, std::string const& value){
	rapidjson::StringBuffer s;
	JSON_Writer writer(s);

	make_data_obj(writer, ap);

	writer.Key(DATA_KEY);
	writer.String(value.c_str());

	writer.EndObject();

	return std::move(std::string(s.GetString()));
}

std::string
Factory::create(App ap, Byte_Array const& b){
	rapidjson::StringBuffer s;
	JSON_Writer writer(s);

	make_data_obj(writer, ap);

	writer.Key(DATA_KEY);
	make_data_array(writer, b);

	writer.EndObject();

	return std::move(std::string(s.GetString()));
}

std::string
Factory::create(App ap, rapidjson::Value& doc){
	rapidjson::StringBuffer s;
	JSON_Writer writer(s);
	rapidjson::Document main_doc;

	main_doc.SetObject();
	main_doc.AddMember(MESSAGE_APP_KEY, rapidjson::Value(app(ap),
			main_doc.GetAllocator()).Move(), main_doc.GetAllocator());
	main_doc.AddMember(MESSAGE_MID_KEY, Core::get_mid(),
				main_doc.GetAllocator());
	main_doc.AddMember(MESSAGE_UID_KEY, Core::server_user_id,
				main_doc.GetAllocator());
	main_doc.AddMember(MESSAGE_TIME_KEY, time(),
			main_doc.GetAllocator());
	main_doc.AddMember(MESSAGE_TYPE_KEY,
			rapidjson::Value(type(Type::data), main_doc.GetAllocator()).Move(),
			main_doc.GetAllocator());
	main_doc.AddMember(DATA_KEY, doc, main_doc.GetAllocator());

	main_doc.Accept(writer);

	return std::string(s.GetString());
}

std::string
Factory::create(App ap, std::vector<std::string> const& value){
	rapidjson::StringBuffer s;
	JSON_Writer writer(s);

	make_data_obj(writer, ap);

	writer.Key(DATA_KEY);
	make_string_array(writer, value);

	writer.EndObject();

	return std::string(s.GetString());
}


//Control
std::string
Factory::create(App ap, Control c){
	rapidjson::StringBuffer s;
	JSON_Writer writer(s);

	make_control_obj(writer, ap, c);

	writer.EndObject();

	return std::move(std::string(s.GetString()));
}

std::string
Factory::create(App ap, Control c, int value){
	rapidjson::StringBuffer s;
	JSON_Writer writer(s);

	make_control_obj(writer, ap, c);

	writer.Key(DATA_KEY);
	writer.Int(value);

	writer.EndObject();

	return std::move(std::string(s.GetString()));
}

std::string
Factory::create(App ap, Control c, unsigned value){
	rapidjson::StringBuffer s;
	JSON_Writer writer(s);

	make_control_obj(writer, ap, c);

	writer.Key(DATA_KEY);
	writer.Uint(value);

	writer.EndObject();

	return std::move(std::string(s.GetString()));
}

std::string
Factory::create(App ap, Control c, std::string const& value){
	rapidjson::StringBuffer s;
	JSON_Writer writer(s);

	make_control_obj(writer, ap, c);

	writer.Key(DATA_KEY);
	writer.String(value.c_str());

	writer.EndObject();

	return std::move(std::string(s.GetString()));
}

std::string
Factory::create(App ap, Control c, Byte_Array const& b){
	rapidjson::StringBuffer s;
	JSON_Writer writer(s);

	make_control_obj(writer, ap, c);

	writer.Key(DATA_KEY);
	make_data_array(writer, b);

	writer.EndObject();

	return std::move(std::string(s.GetString()));
}

std::string
Factory::create(App ap, Control c, rapidjson::Document& doc){
	rapidjson::StringBuffer s;
	JSON_Writer writer(s);
	rapidjson::Document main_doc;

	main_doc.SetObject();
	main_doc.AddMember(MESSAGE_APP_KEY, rapidjson::Value(app(ap),
			main_doc.GetAllocator()).Move(), main_doc.GetAllocator());
	main_doc.AddMember(MESSAGE_MID_KEY, Core::get_mid(),
				main_doc.GetAllocator());
	main_doc.AddMember(MESSAGE_UID_KEY, Core::server_user_id,
				main_doc.GetAllocator());
	main_doc.AddMember(MESSAGE_TIME_KEY, time(),
			main_doc.GetAllocator());
	main_doc.AddMember(MESSAGE_TYPE_KEY,
			rapidjson::Value(type(Type::control), main_doc.GetAllocator()).Move(),
			main_doc.GetAllocator());
	main_doc.AddMember(CONTROL_TYPE_KEY,
			rapidjson::Value(control(c), main_doc.GetAllocator()).Move(),
			main_doc.GetAllocator());
	main_doc.AddMember(DATA_KEY, doc, main_doc.GetAllocator());

	main_doc.Accept(writer);

	return std::move(std::string(s.GetString()));
}

std::string
Factory::create(App ap, Control c, std::vector<std::string> const& value){
	rapidjson::StringBuffer s;
	JSON_Writer writer(s);

	make_control_obj(writer, ap, c);

	writer.Key(DATA_KEY);
	make_string_array(writer, value);

	writer.EndObject();

	return std::move(std::string(s.GetString()));
}

static const char* app(App a){
	Core::App app;
	if(Core::Dispatcher::app(app, a)) return app.name();

	return NULL;
}

static const char* type(Type t){
	switch(t){
		case Type::data:
			return MESSAGE_TYPE_DATA;
		case Type::control:
			return MESSAGE_TYPE_CONTROL;
	}

	return NULL;
}

static const char* control(Control c){
	switch(c){
		case Control::status:
			return CONTROL_TYPE_STATUS;
		case Control::config:
			return CONTROL_TYPE_CONFIG;
		case Control::close:
			return CONTROL_TYPE_CLOSE;
		case Control::open:
			return CONTROL_TYPE_OPEN;
		case Control::error:
			return CONTROL_TYPE_ERROR;
		case Control::custom:
			return CONTROL_TYPE_CUSTOM;
	}

	return NULL;
}

static void make_data_obj(
		JSON_Writer& writer,
		App ap)
{
	writer.StartObject();

	writer.Key(MESSAGE_APP_KEY);
	writer.String(app(ap));

	writer.Key(MESSAGE_MID_KEY);
	writer.Uint64(Core::get_mid());

	writer.Key(MESSAGE_UID_KEY);
	writer.Int(Core::server_user_id);

	writer.Key(MESSAGE_TIME_KEY);
	writer.Uint64(time());

	writer.Key(MESSAGE_TYPE_KEY);
	writer.String(type(Type::data));
}

static void make_control_obj(
		JSON_Writer& writer,
		App ap,
		Control con)
{
	writer.StartObject();

	writer.Key(MESSAGE_APP_KEY);
	writer.String(app(ap));

	writer.Key(MESSAGE_MID_KEY);
	writer.Uint64(Core::get_mid());

	writer.Key(MESSAGE_UID_KEY);
	writer.Int(Core::server_user_id);

	writer.Key(MESSAGE_TIME_KEY);
	writer.Uint64(time());

	writer.Key(MESSAGE_TYPE_KEY);
	writer.String(type(Type::control));

	writer.Key(CONTROL_TYPE_KEY);
	writer.String(control(con));
}

static void make_data_array(JSON_Writer& writer,
		Byte_Array const& b){
	writer.StartArray();
	for(auto byte : b){
		writer.Uint(byte);
	}
	writer.EndArray();
}

static void make_string_array(JSON_Writer& writer,
	std::vector<std::string> const& value) {
	writer.StartArray();
	for (auto const& v : value) {
		writer.String(v.c_str());
	}
	writer.EndArray();
}

/*
template<template<class> class Container_s>
static void make_string_array(JSON_Writer& writer,
		Container_s<std::string> const& value){
	writer.StartArray();
	for(auto v : value){
		writer.String(v.c_str());
	}
	writer.EndArray();
}
*/
}//Message
