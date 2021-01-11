#include "../parser.hpp"
#include "../types.hpp"
#include "defines.hpp"

namespace Apps{
namespace Monitor{

static bool check_status_message(const rapidjson::Document& doc);

bool parse_app(const rapidjson::Document& doc, Message_Monitor_Data& data)
{
	if(!check_status_message(doc)) return false;

	if(std::string(doc[DATA_KEY][MONITOR_APP_TYPE].GetString())== MONITOR_APP_TYPE_CANCEL){
		data.type = Monitor_Type::cancel;
		data.cont = false;
		return true;
	} data.type = Monitor_Type::set;

	data.sample_time = doc[DATA_KEY][MONITOR_APP_SAMPLE_TIME].GetInt();
	data.cont = doc[DATA_KEY][MONITOR_APP_CONT].GetBool();

	return true;
}

static bool check_status_message(const rapidjson::Document& doc){
	if(!(doc.HasMember(DATA_KEY) && doc[DATA_KEY].IsObject())) return false;

	auto const& d = doc[DATA_KEY];
	if(!(d.IsObject() &&
			(d.HasMember(MONITOR_APP_TYPE) && d[MONITOR_APP_TYPE].IsString()))){
		return false;
	}

	if(std::string(d[MONITOR_APP_TYPE].GetString()) == MONITOR_APP_TYPE_SET){
		return (d.HasMember(MONITOR_APP_SAMPLE_TIME) && d[MONITOR_APP_SAMPLE_TIME].IsInt()) &&
				(d.HasMember(MONITOR_APP_CONT) && d[MONITOR_APP_CONT].IsBool());
	}

	return true;
}

}//Monitor
}//Apps

