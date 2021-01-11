#include "util/json_helper.hpp"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>

#include <iostream>

namespace Util{

std::string json_2_string(const rapidjson::Value& d, bool pretty /* = true */){
	rapidjson::StringBuffer s;

	if(pretty){
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
		d.Accept(writer);
	} else {
		rapidjson::Writer<rapidjson::StringBuffer> writer(s);
		d.Accept(writer);
	}

	return std::move(std::string(s.GetString()));
}


void print_json(const rapidjson::Value& d, bool pretty /* = true */){
	std::cout << json_2_string(d, pretty);
}

bool json_2_byte_array(const rapidjson::Value& doc, Byte_Array& data){
	if(doc.IsArray()){
		for (rapidjson::Value::ConstValueIterator itr = doc.Begin(); itr != doc.End(); ++itr) {
			data.push_back(static_cast<uint8_t>(itr->GetUint()));
		}
		return true;
	}
	if(doc.IsObject()){
		for (rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin();
		    itr != doc.MemberEnd(); ++itr){
				data.push_back(static_cast<uint8_t>(itr->value.GetUint()));
		}
		return true;
	}
	return false;
}


}//Util
