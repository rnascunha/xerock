#include "udp_client.hpp"
#include "parser.hpp"

#include "types.hpp"

#include "make.hpp"

#include "core/helpers.hpp"
#include "core/error.hpp"

#include "my_async.hpp"

//#include <iostream>

namespace Apps{
namespace UDP_Client{

static void message_data(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
		std::shared_ptr<UDP_Container> clients);
static void message_close(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
		std::shared_ptr<UDP_Container> clients);
static void message_open(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
		std::shared_ptr<UDP_Container> clients);
static void message_status(std::shared_ptr<UDP_Container> clients);

void udp_client_app(std::shared_ptr<Core::Propagator> ws,
		Message::Main_Data const& main,
		Message::Type_Recv const& doc,
		std::shared_ptr<UDP_Container> clients)
{
	if(main.type == Message::Type::control)
	{
		//control
		switch(main.control)
		{
			case Message::Control::open:
				message_open(ws, doc, clients);
				break;
			case Message::Control::close:
				message_close(ws, doc, clients);
				break;
			case Message::Control::status:
				message_status(clients);
				break;
			default:
				Core::fail_message(ws, Message::App::udp_client, make_error_code(Message_Errc::ILL_FORMED));
				break;
		}
	} else {
		//data
		message_data(ws, doc, clients);
	}
}

/**
 *
 *
 *
 */

static void message_data(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
		std::shared_ptr<UDP_Container> clients)
{
	Message_Data data;
	if(!parser_data(doc, data))
	{
		Core::fail_message(ws, Message::App::udp_client, make_error_code(Message_Errc::ILL_FORMED));
		return ;
	}

	boost::system::error_code ec;
	clients->write(data.local, data.data, ec);
	if(ec)
	{
		Core::fail_message(ws, Message::App::tcp_client, ec);
		return;
	}
}

static void message_status(std::shared_ptr<UDP_Container> clients)
{
	Core::Propagator::write_all(Byte_Array(make_status_message(clients)));
}

static void message_close(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
		std::shared_ptr<UDP_Container> clients)
{
	Message_Close data;

	if(!parser_close(doc, data))
	{
		Core::fail_message(ws, Message::App::udp_client, make_error_code(Message_Errc::ILL_FORMED));
		return;
	}

	boost::system::error_code ec;
	clients->close(data.local, ec);
	if(ec)
	{
		Core::fail_message(ws, Message::App::udp_client, ec);
		return;
	}
	message_status(clients);
}

static void message_open(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
		std::shared_ptr<UDP_Container> clients)
{
	Message_Open data;

	if(!parser_open(doc, data))
	{
		Core::fail_message(ws, Message::App::udp_client, make_error_code(Message_Errc::ILL_FORMED));
		return;
	}

	clients->open(data.remote);
	message_status(clients);
}

}//UDP_Client
}//Apps

