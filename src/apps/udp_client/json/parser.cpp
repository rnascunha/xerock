#include "../parser.hpp"
#include "defines.hpp"
#include "core/json/defines.hpp"

#include <boost/asio.hpp>
#include "rapidjson/document.h"

#include "util/json_helper.hpp"

//https://github.com/Tencent/rapidjson/issues/1448
//#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
//#undef GetObject
//#endif

namespace Apps{
namespace UDP_Client{

#ifndef CONFIG_VALID_TCP_PORT_MIN
#define CONFIG_VALID_TCP_PORT_MIN	1024
#endif

#ifndef CONFIG_VALID_TCP_PORT_MAX
#define CONFIG_VALID_TCP_PORT_MAX	65536
#endif

static bool is_valid_port(unsigned port)
{
	return port > CONFIG_VALID_TCP_PORT_MIN && port < CONFIG_VALID_TCP_PORT_MAX;
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
	if(!(d.HasMember(MESSAGE_UDP_CLIENT_DATA_KEY)
		&& (d[MESSAGE_UDP_CLIENT_DATA_KEY].IsObject() || d[MESSAGE_UDP_CLIENT_DATA_KEY].IsString())))
	{
		return false;
	}

	if(!(d.HasMember(MESSAGE_UDP_CLIENT_LOCAL) && d[MESSAGE_UDP_CLIENT_LOCAL].IsObject())) return false;

	auto const& local = d[MESSAGE_UDP_CLIENT_LOCAL];
	return (local.HasMember(MESSAGE_UDP_CLIENT_DATA_LOCAL_ADDR_KEY)
		&& local[MESSAGE_UDP_CLIENT_DATA_LOCAL_ADDR_KEY].IsString())
		&& (local.HasMember(MESSAGE_UDP_CLIENT_DATA_LOCAL_PORT_KEY)
		&& local[MESSAGE_UDP_CLIENT_DATA_LOCAL_PORT_KEY].IsUint());
}

static bool get_data(const rapidjson::Value& doc, Message_Data& data)
{
	{
		auto const& local = doc[MESSAGE_UDP_CLIENT_DATA_KEY];
		unsigned port = local[MESSAGE_UDP_CLIENT_DATA_LOCAL_PORT_KEY].GetUint();
		if(!is_valid_port(port)) return false;

		boost::system::error_code ec;
		auto const address = boost::asio::ip::make_address(local[MESSAGE_UDP_CLIENT_DATA_LOCAL_ADDR_KEY].GetString(), ec);
		if(ec) return false;

		data.local = boost::asio::ip::udp::endpoint{address, static_cast<unsigned short>(port)};
	}

	if(doc[MESSAGE_UDP_CLIENT_LOCAL].IsObject())
			::Util::json_2_byte_array(doc[MESSAGE_UDP_CLIENT_LOCAL], data.data);
	else data.data = doc[MESSAGE_UDP_CLIENT_LOCAL].GetString();

	return true;
}

/**
 * Open
 */
static bool check_open(const rapidjson::Document& doc)
{
	if(!(doc.HasMember(DATA_KEY) && doc[DATA_KEY].IsObject())) return false;

	auto const& d = doc[DATA_KEY];
	return (d.HasMember(MESSAGE_UDP_CLIENT_OPEN_PORT_KEY)
			&& d[MESSAGE_UDP_CLIENT_OPEN_PORT_KEY].IsUint())
			&&
			(d.HasMember(MESSAGE_UDP_CLIENT_OPEN_ADDR_KEY)
			&& d[MESSAGE_UDP_CLIENT_OPEN_ADDR_KEY].IsString());
}

static bool get_open(const rapidjson::Value& doc, Message_Open& data)
{
	uint16_t port = static_cast<uint16_t>(doc[MESSAGE_UDP_CLIENT_OPEN_PORT_KEY].GetUint());
	if(!is_valid_port(port)) return false;

	boost::system::error_code ec;
	auto const address = boost::asio::ip::make_address(
			doc[MESSAGE_UDP_CLIENT_OPEN_ADDR_KEY].GetString(), ec);
	if(ec) return false;

	data.remote = boost::asio::ip::udp::endpoint{address, static_cast<unsigned short>(port)};

	return true;
}

/**
 * Close
 */
static bool check_close(const Message::Type_Recv& doc)
{
	if(!(doc.HasMember(DATA_KEY) && doc[DATA_KEY].IsObject())) return false;

	auto const& d = doc[DATA_KEY];
	return (d.HasMember(MESSAGE_UDP_CLIENT_CLOSE_PORT_KEY)
			&& d[MESSAGE_UDP_CLIENT_CLOSE_PORT_KEY].IsUint())
			&&
			(d.HasMember(MESSAGE_UDP_CLIENT_CLOSE_ADDR_KEY)
			&& d[MESSAGE_UDP_CLIENT_CLOSE_ADDR_KEY].IsString());
}

static bool get_close(const rapidjson::Value& doc, Message_Close& data){
	unsigned port = doc[MESSAGE_UDP_CLIENT_CLOSE_PORT_KEY].GetUint();
	if(!is_valid_port(port)) return false;

	boost::system::error_code ec;
	auto const address = boost::asio::ip::make_address(doc[MESSAGE_UDP_CLIENT_CLOSE_ADDR_KEY].GetString(), ec);
	if(ec) return false;

	data.local = boost::asio::ip::udp::endpoint{address, static_cast<unsigned short>(port)};

	return true;
}

}//UDP_Client
}//Apps
