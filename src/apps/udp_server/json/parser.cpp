#include "../parser.hpp"
#include "defines.hpp"
#include "core/json/defines.hpp"

#include <boost/asio.hpp>
#include "rapidjson/document.h"

#include "util/json_helper.hpp"

//https://github.com/Tencent/rapidjson/issues/1448
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#undef GetObject
#endif

namespace Apps{
namespace UDP_Server{

#ifndef CONFIG_VALID_UDP_PORT_MIN
#define CONFIG_VALID_UDP_PORT_MIN	1024
#endif

#ifndef CONFIG_VALID_UDP_PORT_MAX
#define CONFIG_VALID_UDP_PORT_MAX	65536
#endif

static bool is_valid_port(unsigned port)
{
	return port > CONFIG_VALID_UDP_PORT_MIN && port < CONFIG_VALID_UDP_PORT_MAX;
}

/**
 * Data
 */
static bool check_data(const rapidjson::Document& doc);
static bool get_data(const rapidjson::Value& doc, Message_Data& data);
bool parser_data(const rapidjson::Document& doc, Message_Data& data)
{
	if(!check_data(doc)) return false;
	return get_data(doc[DATA_KEY], data);
}

/**
 * Open
 */
static bool check_open(const rapidjson::Document& doc);
static bool get_open(const rapidjson::Value& doc, Message_Open& data);
bool parser_open(const rapidjson::Document& doc, Message_Open& data)
{
	if(!check_open(doc)) return false;
	return get_open(doc[DATA_KEY], data);
}

/**
 * Close
 */
static bool check_close(const Message::Type_Recv& doc);
static bool get_close(const rapidjson::Value& doc, Message_Close& data);
bool parser_close(const Message::Type_Recv& doc, Message_Close& data)
{
	if(!check_close(doc)) return false;
	return get_close(doc[DATA_KEY], data);
}

/**
 *
 *
 *
 *
 */

/**
 * Data
 */
static bool check_data(const rapidjson::Document& doc)
{
	if(!(doc.HasMember(DATA_KEY) && doc[DATA_KEY].IsObject())) return false;

	auto const& d = doc[DATA_KEY];
	if(!((d.HasMember(MESSAGE_UDP_SERVER_DATA_FROM)
				&& d[MESSAGE_UDP_SERVER_DATA_FROM].IsObject()) &&
				d[MESSAGE_UDP_SERVER_DATA_FROM].HasMember(MESSAGE_UDP_SERVER_ADDR_KEY) &&
				d[MESSAGE_UDP_SERVER_DATA_FROM][MESSAGE_UDP_SERVER_ADDR_KEY].IsString() &&
				d[MESSAGE_UDP_SERVER_DATA_FROM].HasMember(MESSAGE_UDP_SERVER_PORT_KEY) &&
				d[MESSAGE_UDP_SERVER_DATA_FROM][MESSAGE_UDP_SERVER_PORT_KEY].IsUint()) &&
			((d.HasMember(MESSAGE_UDP_SERVER_DATA_TO) &&
					d[MESSAGE_UDP_SERVER_DATA_TO].IsArray()))) return false;

	auto const& clients = d[MESSAGE_UDP_SERVER_DATA_TO];
	for (rapidjson::Value::ConstValueIterator itr = clients.Begin(); itr != clients.End(); ++itr)
	{
		if(!itr->IsObject()) return false;
		auto const& client = itr->GetObject();
		if(!((client.HasMember(MESSAGE_UDP_SERVER_ADDR_KEY)
				&& client[MESSAGE_UDP_SERVER_ADDR_KEY].IsString()) &&
				(client.HasMember(MESSAGE_UDP_SERVER_PORT_KEY) &&
						client[MESSAGE_UDP_SERVER_PORT_KEY].IsUint()))) return false;
	}
	return (d.HasMember(MESSAGE_UDP_SERVER_DATA_KEY) && (d[MESSAGE_UDP_SERVER_DATA_KEY].IsObject()
										|| d[MESSAGE_UDP_SERVER_DATA_KEY].IsString()));
}

static bool get_data(const rapidjson::Value& doc, Message_Data& data)
{
	{
		unsigned port = doc[MESSAGE_UDP_SERVER_DATA_FROM][MESSAGE_UDP_SERVER_PORT_KEY].GetUint();
		if(!is_valid_port(port)) return false;

		boost::system::error_code ec;
		auto const address = boost::asio::ip::make_address(doc[MESSAGE_UDP_SERVER_DATA_FROM][MESSAGE_UDP_SERVER_ADDR_KEY].GetString(), ec);
		if(ec) return false;

		data.from = boost::asio::ip::udp::endpoint{address, static_cast<unsigned short>(port)};
	}
	{
		auto const& clients = doc[MESSAGE_UDP_SERVER_DATA_TO];
		for (rapidjson::Value::ConstValueIterator itr = clients.Begin(); itr != clients.End(); ++itr)
		{
			auto const& client = itr->GetObject();
			unsigned port = client[MESSAGE_UDP_SERVER_PORT_KEY].GetUint();
			if(!is_valid_port(port)) return false;

			boost::system::error_code ec;
			auto const address = boost::asio::ip::make_address(client[MESSAGE_UDP_SERVER_ADDR_KEY].GetString(), ec);
			if(ec) return false;

			data.to.push_back(boost::asio::ip::udp::endpoint{address, static_cast<unsigned short>(port)});
		}
	}

	if(doc[MESSAGE_UDP_SERVER_DATA_KEY].IsObject())
			::Util::json_2_byte_array(doc[MESSAGE_UDP_SERVER_DATA_KEY], data.data);
	else data.data = doc[MESSAGE_UDP_SERVER_DATA_KEY].GetString();

	return true;
}

/**
 * Open
 */
static bool check_open(const rapidjson::Document& doc)
{
	if(!(doc.HasMember(DATA_KEY) && doc[DATA_KEY].IsObject())) return false;

	auto const& d = doc[DATA_KEY];
	return (d.HasMember(MESSAGE_UDP_SERVER_OPEN_PORT_KEY)
			&& d[MESSAGE_UDP_SERVER_OPEN_PORT_KEY].IsUint())
			&&
			(d.HasMember(MESSAGE_UDP_SERVER_OPEN_ADDR_KEY)
			&& d[MESSAGE_UDP_SERVER_OPEN_ADDR_KEY].IsString());
}

static bool get_open(const rapidjson::Value& doc, Message_Open& data)
{
	unsigned port = doc[MESSAGE_UDP_SERVER_PORT_KEY].GetUint();
	if(!is_valid_port(port)) return false;

	boost::system::error_code ec;
	auto const address = boost::asio::ip::make_address(doc[MESSAGE_UDP_SERVER_ADDR_KEY].GetString(), ec);
	if(ec) return false;

	data.ep = boost::asio::ip::udp::endpoint{address, static_cast<unsigned short>(port)};

	return true;
}

/**
 * Close
 */
static bool check_close(const Message::Type_Recv& doc)
{
	if(!(doc.HasMember(DATA_KEY) && doc[DATA_KEY].IsObject())) return false;

	auto const& d = doc[DATA_KEY];
	return (d.HasMember(MESSAGE_UDP_SERVER_CLOSE_PORT_KEY)
			&& d[MESSAGE_UDP_SERVER_CLOSE_PORT_KEY].IsUint())
			&&
			(d.HasMember(MESSAGE_UDP_SERVER_CLOSE_ADDR_KEY)
			&& d[MESSAGE_UDP_SERVER_CLOSE_ADDR_KEY].IsString());
}

static bool get_close(const rapidjson::Value& doc, Message_Close& data){
	unsigned port = doc[MESSAGE_UDP_SERVER_CLOSE_PORT_KEY].GetUint();
	if(!is_valid_port(port)) return false;

	boost::system::error_code ec;
	auto const address = boost::asio::ip::make_address(doc[MESSAGE_UDP_SERVER_CLOSE_ADDR_KEY].GetString(), ec);
	if(ec) return false;

	data.ep = boost::asio::ip::udp::endpoint{address, static_cast<unsigned short>(port)};

	return true;
}

}//UDP_Server
}//Apps
