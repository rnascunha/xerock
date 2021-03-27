#include "../make.hpp"
#include "rapidjson/document.h"
#include "defines.hpp"

namespace Apps{
namespace UDP_Server{

Message::Type_Send make_status_message(std::shared_ptr<Container> servers)
{
	rapidjson::Document doc;

	doc.SetObject();

	rapidjson::Value d;
	d.SetArray();

	auto const eps = servers->endpoints();
	for(auto const& ep : eps){
		rapidjson::Value ep_v;
		ep_v.SetObject();
		ep_v.AddMember(MESSAGE_UDP_SERVER_STATUS_EP_ADDR_KEY, rapidjson::Value(ep.address().to_string().c_str(), doc.GetAllocator()).Move(), doc.GetAllocator());
		ep_v.AddMember(MESSAGE_UDP_SERVER_STATUS_EP_PORT_KEY, ep.port(), doc.GetAllocator());

		d.PushBack(ep_v, doc.GetAllocator());
	}

	doc.AddMember(MESSAGE_UDP_SERVER_STATUS_LIST, d, doc.GetAllocator());
	return Message::Factory::create(Message::App::udp_server,
						Message::Control::status,
						doc);
}

Message::Type_Send make_received_message(
		boost::asio::ip::udp::endpoint const& to,
		boost::asio::ip::udp::endpoint const& from,
		Byte_Array const& data)
{
	rapidjson::Document doc;

	doc.SetObject();

	//From
	rapidjson::Value from_;
	from_.SetObject();
	from_.AddMember(MESSAGE_UDP_SERVER_ADDR_KEY, rapidjson::Value(from.address().to_string().c_str(), doc.GetAllocator()).Move(), doc.GetAllocator());
	from_.AddMember(MESSAGE_UDP_SERVER_PORT_KEY, from.port(), doc.GetAllocator());

	doc.AddMember(MESSAGE_UDP_SERVER_DATA_FROM, from_, doc.GetAllocator());

	//To
	rapidjson::Value to_;
	to_.SetObject();
	to_.AddMember(MESSAGE_UDP_SERVER_ADDR_KEY, rapidjson::Value(to.address().to_string().c_str(), doc.GetAllocator()).Move(), doc.GetAllocator());
	to_.AddMember(MESSAGE_UDP_SERVER_PORT_KEY, to.port(), doc.GetAllocator());

	doc.AddMember(MESSAGE_UDP_SERVER_DATA_TO, to_, doc.GetAllocator());

	rapidjson::Value data_arr;
	data_arr.SetArray();
	for(auto const& d : data){
		data_arr.PushBack(d, doc.GetAllocator());
	}
	doc.AddMember(MESSAGE_UDP_SERVER_DATA_KEY, data_arr, doc.GetAllocator());

	return Message::Factory::create(Message::App::udp_server, doc);
}

}//UDP_Server
}//Apps
