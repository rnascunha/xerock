#include "../parser.hpp"
#include "defines.hpp"
#include "../../main/json/defines.hpp"

#include "util/json_helper.hpp"

namespace Apps{
namespace Echo{

//Echo
static bool check_echo_message(const rapidjson::Document& doc);
static bool get_echo_message(const rapidjson::Value& doc, Echo_Data& data);

bool parser_echo(const rapidjson::Document& doc, Echo_Data& data){
	if(!check_echo_message(doc)) return false;
	if(!get_echo_message(doc[DATA_KEY], data)) return false;
	return true;
}

static bool check_echo_message(const rapidjson::Document& doc){
	if(!(doc.HasMember(DATA_KEY) && doc[DATA_KEY].IsObject())) return false;

	auto const& d = doc[DATA_KEY];
	return d.IsObject() &&
			(d.HasMember(ECHO_TYPE_KEY) && d[ECHO_TYPE_KEY].IsString()) &&
			(d.HasMember(ECHO_DATA_KEY));
}

static bool get_echo_message(const rapidjson::Value& doc, Echo_Data& data){
	{
		std::string type = doc[ECHO_TYPE_KEY].GetString();
		if(type == ECHO_TYPE_STRING){
			data.type = Echo_Type::string;
		} else if(type ==  ECHO_TYPE_HEX){
			data.type = Echo_Type::hex;
		} else if(type == ECHO_TYPE_BIN){
			data.type = Echo_Type::binary;
		} else if(type == ECHO_TYPE_OCTO){
			data.type = Echo_Type::octo;
		} else
			return false;
	}

	::Util::json_2_byte_array(doc[DATA_KEY], data.data);

	return true;
}

}//Echo
}//Apps

