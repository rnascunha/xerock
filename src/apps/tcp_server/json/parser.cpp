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
namespace TCP_Server{

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
 * Close clientes
 */
static bool check_close_clients(const rapidjson::Document& doc);
static bool get_close_clients(const rapidjson::Value& doc, Message_Close_Client& data);
bool parser_close_clients(const rapidjson::Document& doc, Message_Close_Client& data)
{
	if(!check_close_clients(doc)) return false;
	return get_close_clients(doc[DATA_KEY], data);
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
static bool check_data(const rapidjson::Document& doc){
	if(!(doc.HasMember(DATA_KEY) && doc[DATA_KEY].IsObject())) return false;

	auto const& d = doc[DATA_KEY];
	if(!((d.HasMember(MESSAGE_TCP_SERVER_DATA_FROM)
				&& d[MESSAGE_TCP_SERVER_DATA_FROM].IsObject()) &&
				d[MESSAGE_TCP_SERVER_DATA_FROM].HasMember(MESSAGE_TCP_SERVER_ADDR_KEY) &&
				d[MESSAGE_TCP_SERVER_DATA_FROM][MESSAGE_TCP_SERVER_ADDR_KEY].IsString() &&
				d[MESSAGE_TCP_SERVER_DATA_FROM].HasMember(MESSAGE_TCP_SERVER_PORT_KEY) &&
				d[MESSAGE_TCP_SERVER_DATA_FROM][MESSAGE_TCP_SERVER_PORT_KEY].IsUint()) &&
			((d.HasMember(MESSAGE_TCP_SERVER_DATA_TO) &&
					d[MESSAGE_TCP_SERVER_DATA_TO].IsArray()))) return false;

	auto const& clients = d[MESSAGE_TCP_SERVER_DATA_TO];
	for (rapidjson::Value::ConstValueIterator itr = clients.Begin(); itr != clients.End(); ++itr)
	{
		if(!itr->IsObject()) return false;
		auto const& client = itr->GetObject();
		if(!((client.HasMember(MESSAGE_TCP_SERVER_ADDR_KEY)
				&& client[MESSAGE_TCP_SERVER_ADDR_KEY].IsString()) &&
				(client.HasMember(MESSAGE_TCP_SERVER_PORT_KEY) &&
						client[MESSAGE_TCP_SERVER_PORT_KEY].IsUint()))) return false;
	}
	return (d.HasMember(MESSAGE_TCP_SERVER_DATA_KEY) && (d[MESSAGE_TCP_SERVER_DATA_KEY].IsObject()
										|| d[MESSAGE_TCP_SERVER_DATA_KEY].IsString()));
}

static bool get_data(const rapidjson::Value& doc, Message_Data& data)
{
	{
		unsigned port = doc[MESSAGE_TCP_SERVER_DATA_FROM][MESSAGE_TCP_SERVER_PORT_KEY].GetUint();
		if(!is_valid_port(port)) return false;

		boost::system::error_code ec;
		auto const address = boost::asio::ip::make_address(doc[MESSAGE_TCP_SERVER_DATA_FROM][MESSAGE_TCP_SERVER_ADDR_KEY].GetString(), ec);
		if(ec) return false;

		data.from = boost::asio::ip::tcp::endpoint{address, static_cast<unsigned short>(port)};
	}
	{
		auto const& clients = doc[MESSAGE_TCP_SERVER_DATA_TO];
		for (rapidjson::Value::ConstValueIterator itr = clients.Begin(); itr != clients.End(); ++itr)
		{
			auto const& client = itr->GetObject();
			unsigned port = client[MESSAGE_TCP_SERVER_PORT_KEY].GetUint();
			if(!is_valid_port(port)) return false;

			boost::system::error_code ec;
			auto const address = boost::asio::ip::make_address(client[MESSAGE_TCP_SERVER_ADDR_KEY].GetString(), ec);
			if(ec) return false;

			data.to.push_back(boost::asio::ip::tcp::endpoint{address, static_cast<unsigned short>(port)});
		}
	}

	if(doc[MESSAGE_TCP_SERVER_DATA_KEY].IsObject())
			::Util::json_2_byte_array(doc[MESSAGE_TCP_SERVER_DATA_KEY], data.data);
	else data.data = doc[MESSAGE_TCP_SERVER_DATA_KEY].GetString();

	return true;
}

/**
 * Open
 */
static bool check_open(const rapidjson::Document& doc)
{
	if(!(doc.HasMember(DATA_KEY) && doc[DATA_KEY].IsObject())) return false;

	auto const& d = doc[DATA_KEY];
	return (d.HasMember(MESSAGE_TCP_SERVER_OPEN_PORT_KEY)
			&& d[MESSAGE_TCP_SERVER_OPEN_PORT_KEY].IsUint())
			&&
			(d.HasMember(MESSAGE_TCP_SERVER_OPEN_SECURE_KEY)
			&& d[MESSAGE_TCP_SERVER_OPEN_SECURE_KEY].IsString())
			&&
			(d.HasMember(MESSAGE_TCP_SERVER_OPEN_ADDR_KEY)
			&& d[MESSAGE_TCP_SERVER_OPEN_ADDR_KEY].IsString());
}

static bool check_keepalive(const rapidjson::Value& obj)
{
	if(!(obj.HasMember(MESSAGE_TCP_SERVER_OPTIONS)
			&& obj[MESSAGE_TCP_SERVER_OPTIONS].IsObject())) return false;

	auto const& opt = obj[MESSAGE_TCP_SERVER_OPTIONS];
	return (opt.HasMember(MESSAGE_TCP_SERVER_OPTIONS_KEEPALIVE)
			&& opt[MESSAGE_TCP_SERVER_OPTIONS_KEEPALIVE].IsBool())
			&&
			(opt.HasMember(MESSAGE_TCP_SERVER_OPTION_IDLE)
			&& opt[MESSAGE_TCP_SERVER_OPTION_IDLE].IsInt())
			&&
			(opt.HasMember(MESSAGE_TCP_SERVER_OPTION_INTERVAL)
			&& opt[MESSAGE_TCP_SERVER_OPTION_INTERVAL].IsInt())
			&&
			(opt.HasMember(MESSAGE_TCP_SERVER_OPTION_COUNT)
			&& opt[MESSAGE_TCP_SERVER_OPTION_COUNT].IsInt());
}

static void get_keepalive(const rapidjson::Value& opt, Message_Open& data)
{
	data.options.keep_alive = opt[MESSAGE_TCP_SERVER_OPTIONS_KEEPALIVE].GetBool();
	data.options.idle = opt[MESSAGE_TCP_SERVER_OPTION_IDLE].GetInt();
	data.options.interval = opt[MESSAGE_TCP_SERVER_OPTION_INTERVAL].GetInt();
	data.options.count = opt[MESSAGE_TCP_SERVER_OPTION_COUNT].GetInt();
}

static bool get_open(const rapidjson::Value& doc, Message_Open& data)
{
	uint16_t port = static_cast<uint16_t>(doc[MESSAGE_TCP_SERVER_PORT_KEY].GetUint());
	if(!is_valid_port(port)) return false;
	data.port = port;

	data.secure = std::string(doc[MESSAGE_TCP_SERVER_OPEN_SECURE_KEY].GetString()) ==
			MESSAGE_SECURE_TYPE_SSL ? Secure_Type::ssl : Secure_Type::plain;

	data.addr = doc[MESSAGE_TCP_SERVER_OPEN_ADDR_KEY].GetString();

	if(check_keepalive(doc))
		get_keepalive(doc[MESSAGE_TCP_SERVER_OPTIONS], data);

	return true;
}

/**
 * Close
 */
static bool check_close(const Message::Type_Recv& doc)
{
	if(!(doc.HasMember(DATA_KEY) && doc[DATA_KEY].IsObject())) return false;

	auto const& d = doc[DATA_KEY];
	return (d.HasMember(MESSAGE_TCP_SERVER_CLOSE_PORT_KEY)
			&& d[MESSAGE_TCP_SERVER_CLOSE_PORT_KEY].IsUint())
			&&
			(d.HasMember(MESSAGE_TCP_SERVER_CLOSE_ADDR_KEY)
			&& d[MESSAGE_TCP_SERVER_CLOSE_ADDR_KEY].IsString());
}

static bool get_close(const rapidjson::Value& doc, Message_Close& data){
	unsigned port = doc[MESSAGE_TCP_SERVER_CLOSE_PORT_KEY].GetUint();
	if(!is_valid_port(port)) return false;

	boost::system::error_code ec;
	auto const address = boost::asio::ip::make_address(doc[MESSAGE_TCP_SERVER_CLOSE_ADDR_KEY].GetString(), ec);
	if(ec) return false;

	data.endpoint = boost::asio::ip::tcp::endpoint{address, static_cast<unsigned short>(port)};

	return true;
}

/**
 * Close clientes
 */
static bool check_addr(const rapidjson::Value& d){
	return (d.HasMember(MESSAGE_TCP_SERVER_CLOSE_CLIENT_ADDR_KEY)
			&& d[MESSAGE_TCP_SERVER_CLOSE_CLIENT_ADDR_KEY].IsString()) &&
			(d.HasMember(MESSAGE_TCP_SERVER_CLOSE_CLIENT_PORT_KEY)
			&& d[MESSAGE_TCP_SERVER_CLOSE_CLIENT_PORT_KEY].IsUint());;
}

static bool check_close_clients(const rapidjson::Document& doc){
	if(!(doc.HasMember(DATA_KEY) && doc[DATA_KEY].IsObject())) return false;

	auto const& d = doc[DATA_KEY];
	return (d.HasMember(MESSAGE_TCP_SERVER_CLOSE_CLIENT_SERVER_KEY)
			&& d[MESSAGE_TCP_SERVER_CLOSE_CLIENT_SERVER_KEY].IsObject()
			&& check_addr(d[MESSAGE_TCP_SERVER_CLOSE_CLIENT_SERVER_KEY]))
			&&
			d.HasMember(MESSAGE_TCP_SERVER_CLOSE_CLIENT_CLIENT_KEY)
			&& d[MESSAGE_TCP_SERVER_CLOSE_CLIENT_CLIENT_KEY].IsObject()
			&& check_addr(d[MESSAGE_TCP_SERVER_CLOSE_CLIENT_CLIENT_KEY]);
}

static bool get_close_clients(const rapidjson::Value& doc, Message_Close_Client& data)
{
	{
		rapidjson::Value const& server = doc[MESSAGE_TCP_SERVER_CLOSE_CLIENT_SERVER_KEY];

		unsigned port = server[MESSAGE_TCP_SERVER_CLOSE_CLIENT_PORT_KEY].GetUint();
		if(!is_valid_port(port)) return false;

		boost::system::error_code ec;
		auto const address = boost::asio::ip::make_address(server[MESSAGE_TCP_SERVER_CLOSE_CLIENT_ADDR_KEY].GetString(), ec);
		if(ec) return false;

		data.endpoint = boost::asio::ip::tcp::endpoint{address, static_cast<unsigned short>(port)};
	}
	{
		rapidjson::Value const& client = doc[MESSAGE_TCP_SERVER_CLOSE_CLIENT_CLIENT_KEY];
		uint16_t port = static_cast<uint16_t>(client[MESSAGE_TCP_SERVER_CLOSE_CLIENT_PORT_KEY].GetUint());
		if(!is_valid_port(port)) return false;

		boost::system::error_code ec;
		auto const address = boost::asio::ip::make_address(client[MESSAGE_TCP_SERVER_CLOSE_CLIENT_ADDR_KEY].GetString(), ec);
		if(ec) return false;

		data.clients.emplace_back(address, port);
	}

	return true;
}

}//TCP_Server
}//Apps
