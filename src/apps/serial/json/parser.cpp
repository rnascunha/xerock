#include "../parser.hpp"
#include "defines.hpp"
#include "rapidjson/document.h"

#include "util/json_helper.hpp"

namespace Apps{
namespace Serial{

//Serial
static bool check_serial_config_message(const rapidjson::Document& doc);
static bool get_serial_config(const rapidjson::Value& doc, Serial_Config&);
static bool check_serial_close_message(const rapidjson::Document& d);
static bool check_serial_data(const rapidjson::Document& doc);
static bool get_serial_data(const rapidjson::Document& doc, std::string& port, Byte_Array& data);

bool parser_serial_config(const rapidjson::Document& doc, Serial_Config& data){
	if(!check_serial_config_message(doc)) return false;

	auto const& d = doc[DATA_KEY];
	if(!get_serial_config(d, data)) return false;

	return true;
}

bool parser_serial_close(const rapidjson::Document& doc, std::string& port){
	if(!check_serial_close_message(doc)) return false;

	port = doc[DATA_KEY][SERIAL_CLOSE_PORT].GetString();

	return true;
}

bool parser_serial_data(const rapidjson::Document& doc, std::string& port, Byte_Array& data){
	if(!check_serial_data(doc)) return false;
	if(!get_serial_data(doc, port, data)) return false;
	return true;
}

/**
 *
 *
 *
 *
 */
static bool check_serial_close_message(const rapidjson::Document& doc){
	if(!(doc.HasMember(DATA_KEY) && doc[DATA_KEY].IsObject())) return false;

	auto const& d = doc[DATA_KEY];
	return d.IsObject() &&
			(d.HasMember(SERIAL_CLOSE_PORT) && d[SERIAL_CLOSE_PORT].IsString());
}

static bool check_serial_config_message(const rapidjson::Document& d){
	if(!(d.HasMember(DATA_KEY) && d[DATA_KEY].IsObject())) return false;

	auto const& doc = d[DATA_KEY];
	return doc.IsObject() &&
			(doc.HasMember(SERIAL_CONFIG_PORT) && doc[SERIAL_CONFIG_PORT].IsString()) &&
			(doc.HasMember(SERIAL_CONFIG_BAUDRATE) && doc[SERIAL_CONFIG_BAUDRATE].IsUint()) &&
			(doc.HasMember(SERIAL_CONFIG_PARITY) && doc[SERIAL_CONFIG_PARITY].IsUint()) &&
			(doc.HasMember(SERIAL_CONFIG_STOPBIT) && doc[SERIAL_CONFIG_STOPBIT].IsUint()) &&
			(doc.HasMember(SERIAL_CONFIG_CHAR_SIZE) && doc[SERIAL_CONFIG_CHAR_SIZE].IsUint()) &&
			(doc.HasMember(SERIAL_CONFIG_FLOWCONTROL) && doc[SERIAL_CONFIG_FLOWCONTROL].IsUint()) &&
			(doc.HasMember(SERIAL_CONFIG_FC_START) && doc[SERIAL_CONFIG_FC_START].IsUint()) &&
			(doc.HasMember(SERIAL_CONFIG_FC_STOP) && doc[SERIAL_CONFIG_FC_STOP].IsUint());
}

static bool get_serial_config(const rapidjson::Value& doc, Serial_Config& data){
	data.port = doc[SERIAL_CONFIG_PORT].GetString();
	data.baud = doc[SERIAL_CONFIG_BAUDRATE].GetUint();
	data.char_size = boost::asio::serial_port_base::character_size(doc[SERIAL_CONFIG_CHAR_SIZE].GetUint());

	{
		unsigned int sb = doc[SERIAL_CONFIG_STOPBIT].GetUint();
		if(sb == 0){
			data.stopbits = boost::asio::serial_port_base::stop_bits::one;
		} else if(sb == 2){
			data.stopbits = boost::asio::serial_port_base::stop_bits::two;
		} else if(sb == 1){
			data.stopbits = boost::asio::serial_port_base::stop_bits::onepointfive;
		} else
			return false;
	}

	{
		unsigned int parity{doc[SERIAL_CONFIG_PARITY].GetUint()};
		if(parity == SERIAL_CONFIG_PARITY_NONE){
			data.parity = boost::asio::serial_port_base::parity::none;
		} else if(parity == SERIAL_CONFIG_PARITY_ODD){
			data.parity = boost::asio::serial_port_base::parity::odd;
		} else if(parity == SERIAL_CONFIG_PARITY_EVEN){
			data.parity = boost::asio::serial_port_base::parity::even;
	//	} else if(parity == SERIAL_CONFIG_PARITY_SPACE){
	//		data.parity = data.baud = boost::asio::serial_port_base::parity::;
	//	} else if(parity == SERIAL_CONFIG_PARITY_MARK){
	//		data.parity = data.baud = boost::asio::serial_port_base::parity::;
		} else
			return false;
	}

	{
		unsigned int fc{doc[SERIAL_CONFIG_FLOWCONTROL].GetUint()};
		if(fc == SERIAL_CONFIG_FC_NONE){
			data.fc = boost::asio::serial_port_base::flow_control::none;
		} else if(fc == SERIAL_CONFIG_FC_SW){
			data.fc = boost::asio::serial_port_base::flow_control::software;
		} else if(fc == SERIAL_CONFIG_FC_HW){
			data.fc = boost::asio::serial_port_base::flow_control::hardware;
	//	} else if(std::string(doc[SERIAL_CONFIG_FLOWCONTROL].GetString()) == SERIAL_CONFIG_FC_HW_SW){
	//		data.fc = boost::asio::serial_port_base::flow_control::;
		} else
			return false;
	}

	data.fc_start = static_cast<uint8_t>(doc[SERIAL_CONFIG_FC_START].GetUint());
	data.fc_stop = static_cast<uint8_t>(doc[SERIAL_CONFIG_FC_STOP].GetUint());

	return true;
}

static bool check_serial_data(const rapidjson::Document& doc){
	if(!(doc.HasMember(DATA_KEY) && doc[DATA_KEY].IsObject())) return false;

	auto const& d = doc[DATA_KEY];
	return doc.IsObject() &&
			(d.HasMember(SERIAL_DATA_PORT) && d[SERIAL_DATA_PORT].IsString() &&
			d.HasMember(SERIAL_DATA_DATA) && (d[SERIAL_DATA_DATA].IsObject()
												|| d[SERIAL_DATA_DATA].IsString()));
}

static bool get_serial_data(const rapidjson::Document& doc, std::string& port, Byte_Array& data){
	port = doc[DATA_KEY][SERIAL_DATA_PORT].GetString();
	if(doc[DATA_KEY][SERIAL_DATA_DATA].IsObject())
		::Util::json_2_byte_array(doc[DATA_KEY][SERIAL_DATA_DATA], data);
	else data = doc[DATA_KEY][SERIAL_DATA_DATA].GetString();
	return true;
}

}//Serial
}//Apps

