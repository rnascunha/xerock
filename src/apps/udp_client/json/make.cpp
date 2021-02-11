#include "../make.hpp"
#include "rapidjson/document.h"
#include "defines.hpp"

namespace Apps{
namespace UDP_Client{

Message::Type_Send make_status_message(std::shared_ptr<UDP_Container> clients)
{
	rapidjson::Document doc;

	doc.SetArray();

	auto const eps = clients->endpoints();
	for(auto const& [local, remote] : eps)
	{
		rapidjson::Value ep_v, local_v, remote_v;

		ep_v.SetObject();

		//Local
		local_v.SetObject();
		local_v.AddMember(MESSAGE_UDP_CLIENT_STATUS_EP_ADDR_KEY,
				rapidjson::Value(local.address().to_string().c_str(),
				doc.GetAllocator()).Move(), doc.GetAllocator());
		local_v.AddMember(MESSAGE_UDP_CLIENT_STATUS_EP_PORT_KEY,
				local.port(),
				doc.GetAllocator());
		ep_v.AddMember(MESSAGE_UDP_CLIENT_STATUS_EP_LOCAL_KEY, local_v, doc.GetAllocator());

		//Remote
		remote_v.SetObject();
		remote_v.AddMember(MESSAGE_UDP_CLIENT_STATUS_EP_ADDR_KEY,
				rapidjson::Value(remote.address().to_string().c_str(),
				doc.GetAllocator()).Move(), doc.GetAllocator());
		remote_v.AddMember(MESSAGE_UDP_CLIENT_STATUS_EP_PORT_KEY,
				remote.port(),
				doc.GetAllocator());
		ep_v.AddMember(MESSAGE_UDP_CLIENT_STATUS_EP_REMOTE_KEY, remote_v, doc.GetAllocator());

		doc.PushBack(ep_v, doc.GetAllocator());
	}

	return Message::Factory::create(Message::App::udp_client,
						Message::Control::status,
						doc);
}

Message::Type_Send make_received_message(
		boost::asio::ip::udp::endpoint const& local,
		Byte_Array const& data)
{
	rapidjson::Document doc;

	doc.SetObject();

	//From
	rapidjson::Value local_ep;
	local_ep.SetObject();
	local_ep.AddMember(MESSAGE_UDP_CLIENT_DATA_LOCAL_ADDR_KEY, rapidjson::Value(local.address().to_string().c_str(), doc.GetAllocator()).Move(), doc.GetAllocator());
	local_ep.AddMember(MESSAGE_UDP_CLIENT_DATA_LOCAL_PORT_KEY, local.port(), doc.GetAllocator());

	doc.AddMember(MESSAGE_UDP_CLIENT_LOCAL, local_ep, doc.GetAllocator());

	rapidjson::Value data_arr;
	data_arr.SetArray();
	for(auto const& d : data){
		data_arr.PushBack(d, doc.GetAllocator());
	}
	doc.AddMember(MESSAGE_UDP_CLIENT_DATA_KEY, data_arr, doc.GetAllocator());

	return Message::Factory::create(Message::App::udp_client, doc);
}

}//UDP_Client
}//Apps

