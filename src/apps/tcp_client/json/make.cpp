#include "../make.hpp"
#include "rapidjson/document.h"
#include "defines.hpp"

namespace Apps{
namespace TCP_Client{

Message::Type_Send make_status_message(std::shared_ptr<TCP_Container> clients)
{
	rapidjson::Document doc;

	doc.SetObject();

	auto const eps = clients->endpoints();
	for(auto const ep : eps)
	{
		rapidjson::Value ep_v, local_v, remote_v;

		auto [ssl, local, remote] = ep;

		ep_v.SetObject();

		//Secure
		ep_v.AddMember(MESSAGE_TCP_CLIENT_STATUS_EP_SECURE_KEY,
				rapidjson::Value((ssl ?
						MESSAGE_SECURE_TYPE_SSL : MESSAGE_SECURE_TYPE_PLAIN), doc.GetAllocator()).Move(),
				doc.GetAllocator());

		//Local
		local_v.SetObject();
		local_v.AddMember(MESSAGE_TCP_CLIENT_STATUS_EP_ADDR_KEY,
				rapidjson::Value(local.address().to_string().c_str(),
				doc.GetAllocator()).Move(), doc.GetAllocator());
		local_v.AddMember(MESSAGE_TCP_CLIENT_STATUS_EP_PORT_KEY,
				local.port(),
				doc.GetAllocator());
		ep_v.AddMember(MESSAGE_TCP_CLIENT_STATUS_EP_LOCAL_KEY, local_v, doc.GetAllocator());

		//Remote
		remote_v.SetObject();
		remote_v.AddMember(MESSAGE_TCP_CLIENT_STATUS_EP_ADDR_KEY,
				rapidjson::Value(remote.address().to_string().c_str(),
				doc.GetAllocator()).Move(), doc.GetAllocator());
		remote_v.AddMember(MESSAGE_TCP_CLIENT_STATUS_EP_PORT_KEY,
				remote.port(),
				doc.GetAllocator());
		ep_v.AddMember(MESSAGE_TCP_CLIENT_STATUS_EP_LOCAL_KEY, remote_v, doc.GetAllocator());

		doc.PushBack(ep_v, doc.GetAllocator());
	}

	return Message::Factory::create(Message::App::tcp_client,
						Message::Control::status,
						doc);
}

Message::Type_Send make_received_message(
		boost::asio::ip::tcp::endpoint const& local,
		Byte_Array const& data)
{
	rapidjson::Document doc;

	doc.SetObject();

	//From
	rapidjson::Value local_ep;
	local_ep.SetObject();
	local_ep.AddMember(MESSAGE_TCP_CLIENT_DATA_LOCAL_ADDR_KEY, rapidjson::Value(local.address().to_string().c_str(), doc.GetAllocator()).Move(), doc.GetAllocator());
	local_ep.AddMember(MESSAGE_TCP_CLIENT_DATA_LOCAL_PORT_KEY, local.port(), doc.GetAllocator());

	doc.AddMember(MESSAGE_TCP_CLIENT_LOCAL, local_ep, doc.GetAllocator());

	rapidjson::Value data_arr;
	data_arr.SetArray();
	for(auto const& d : data){
		data_arr.PushBack(d, doc.GetAllocator());
	}
	doc.AddMember(MESSAGE_TCP_CLIENT_DATA_KEY, data_arr, doc.GetAllocator());

	return Message::Factory::create(Message::App::tcp_client, doc);
}

}//TCP_Client
}//Apps

