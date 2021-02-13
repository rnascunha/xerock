#include "udp_server.hpp"
#include "parser.hpp"

#include "make.hpp"

#include "core/helpers.hpp"
#include "core/error.hpp"

#include "my_async.hpp"

namespace Apps{
namespace UDP_Server{

static void message_data(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
		std::shared_ptr<Container> servers);
static void message_close(std::shared_ptr<Core::Propagator> ws, Message::Type_Recv const& doc,
		std::shared_ptr<Container> servers);
static void message_open(std::shared_ptr<Core::Propagator> ws, Message::Type_Recv const& doc,
		std::shared_ptr<Container> servers);
static void message_status(std::shared_ptr<Container> servers);

void udp_server_app(std::shared_ptr<Core::Propagator> ws,
		Message::Main_Data const& main,
		Message::Type_Recv const& doc,
		std::shared_ptr<Container> servers)
{
	if(main.type == Message::Type::control){
		//control
		switch(main.control){
			case Message::Control::open:
				message_open(ws, doc, servers);
				break;
			case Message::Control::close:
				message_close(ws, doc, servers);
				break;
			case Message::Control::status:
				message_status(servers);
				break;
			default:
				Core::fail_message(ws, Message::App::udp_server, make_error_code(Message_Errc::ILL_FORMED));
				break;
		}
	} else {
		//data
		message_data(ws, doc, servers);
	}
}

/**
 *
 *
 *
 */

static void message_data(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
		std::shared_ptr<Container> servers)
{
	Message_Data data;
	if(!parser_data(doc, data))
	{
		Core::fail_message(ws, Message::App::udp_server, make_error_code(Message_Errc::ILL_FORMED));
		return ;
	}

	boost::system::error_code ec;
	servers->write(data.from, data.to, data.data, ec);
	if(ec)
	{
		Core::fail_message(ws, Message::App::udp_server, ec);
		return;
	}
}

void message_status(std::shared_ptr<Container> servers)
{
	Core::Propagator::write_all(Byte_Array(make_status_message(servers)), true);
}

static void message_close(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
		std::shared_ptr<Container> servers)
{
	Message_Close data;

	if(!parser_close(doc, data))
	{
		Core::fail_message(ws, Message::App::udp_server, make_error_code(Message_Errc::ILL_FORMED));
		return;
	}

	boost::system::error_code ec;
	servers->close(data.ep, ec);
	if(ec)
	{
		Core::fail_message(ws, Message::App::udp_server, make_error_code(Message_Errc::SERVER_INTERNAL_ERROR));
		return;
	}

	message_status(servers);
}

static void message_open(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
		std::shared_ptr<Container> servers)
{
	Message_Open data;

	if(!parser_open(doc, data))
	{
		Core::fail_message(ws, Message::App::udp_server, make_error_code(Message_Errc::ILL_FORMED));
		return;
	}

	boost::system::error_code ec;
	servers->open(data.ep, ec);
	if(ec)
	{
		Core::fail_message(ws, Message::App::udp_server, ec);
		return;
	}

	message_status(servers);
}

}//UDP_Server
}//Apps
