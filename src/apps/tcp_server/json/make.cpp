#include "../make.hpp"
#include "rapidjson/document.h"
#include "defines.hpp"

namespace Apps{
namespace TCP_Server{

Message::Type_Send make_status_message(std::shared_ptr<TCP_Container> servers)
{
	rapidjson::Document doc;

	doc.SetObject();
	doc.AddMember(MESSAGE_TCP_SERVER_STATUS_SIZE_COUNT_KEY, servers->count(), doc.GetAllocator());

	rapidjson::Value d;
	d.SetArray();

	auto const eps = servers->endpoints();
	for(auto const ep : eps){
		rapidjson::Value ep_v;
		ep_v.SetObject();
		ep_v.AddMember(MESSAGE_TCP_SERVER_STATUS_EP_SECURE_KEY,
				rapidjson::Value((ep.first ?
						MESSAGE_SECURE_TYPE_SSL : MESSAGE_SECURE_TYPE_PLAIN),doc.GetAllocator()).Move(),
				doc.GetAllocator());
		ep_v.AddMember(MESSAGE_TCP_SERVER_STATUS_EP_ADDR_KEY, rapidjson::Value(ep.second.address().to_string().c_str(), doc.GetAllocator()).Move(), doc.GetAllocator());
		ep_v.AddMember(MESSAGE_TCP_SERVER_STATUS_EP_PORT_KEY, ep.second.port(), doc.GetAllocator());

		//AddClients
		rapidjson::Value clients;
		clients.SetArray();
		boost::system::error_code ec;
		auto ep_clients = servers->clients(ep.second, ec);
		for(auto c : ep_clients){
			rapidjson::Value client;
			client.SetObject();
			client.AddMember(MESSAGE_TCP_SERVER_STATUS_EP_ADDR_KEY, rapidjson::Value(c.address().to_string().c_str(), doc.GetAllocator()).Move(), doc.GetAllocator());
			client.AddMember(MESSAGE_TCP_SERVER_STATUS_EP_PORT_KEY, c.port(), doc.GetAllocator());

			clients.PushBack(client, doc.GetAllocator());
		}

		ep_v.AddMember(MESSAGE_TCP_SERVER_STATUS_EP_CLIENTS_KEY, clients, doc.GetAllocator());

		d.PushBack(ep_v, doc.GetAllocator());
	}
	doc.AddMember(MESSAGE_TCP_SERVER_STATUS_EPS_LIST_KEY, d, doc.GetAllocator());
	return Message::Factory::create(Message::App::tcp_server,
						Message::Control::status,
						doc);
}

Message::Type_Send make_tcp_server_received_message(
		boost::asio::ip::tcp::endpoint const& from,
		boost::asio::ip::tcp::endpoint const& to,
		Byte_Array const& data)
{
	rapidjson::Document doc;

	doc.SetObject();

	//From
	rapidjson::Value from_;
	from_.SetObject();
	from_.AddMember(MESSAGE_TCP_SERVER_ADDR_KEY, rapidjson::Value(from.address().to_string().c_str(), doc.GetAllocator()).Move(), doc.GetAllocator());
	from_.AddMember(MESSAGE_TCP_SERVER_PORT_KEY, from.port(), doc.GetAllocator());

	doc.AddMember(MESSAGE_TCP_SERVER_DATA_FROM, from_, doc.GetAllocator());

	//To
	rapidjson::Value to_;
	to_.SetObject();
	to_.AddMember(MESSAGE_TCP_SERVER_ADDR_KEY, rapidjson::Value(to.address().to_string().c_str(), doc.GetAllocator()).Move(), doc.GetAllocator());
	to_.AddMember(MESSAGE_TCP_SERVER_PORT_KEY, to.port(), doc.GetAllocator());

	doc.AddMember(MESSAGE_TCP_SERVER_DATA_TO, to_, doc.GetAllocator());

	rapidjson::Value data_arr;
	data_arr.SetArray();
	for(auto const& d : data){
		data_arr.PushBack(d, doc.GetAllocator());
	}
	doc.AddMember(MESSAGE_TCP_SERVER_DATA_KEY, data_arr, doc.GetAllocator());

	return Message::Factory::create(Message::App::tcp_server, doc);
}

}//TCP_Server
}//Apps
