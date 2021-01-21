#include "tcp_client.hpp"
#include "parser.hpp"

#include "types.hpp"

#include "make.hpp"

#include "core/helpers.hpp"
#include "core/error.hpp"

#include "my_async.hpp"

//#include <iostream>

namespace Apps{
namespace TCP_Client{

static void message_data(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
		std::shared_ptr<TCP_Container> clients);
static void message_close(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
		std::shared_ptr<TCP_Container> clients);
static void message_open(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
		std::shared_ptr<TCP_Container> clients);

void tpc_client_app(std::shared_ptr<Core::Propagator> ws,
		Message::Main_Data const& main,
		Message::Type_Recv const& doc,
		std::shared_ptr<TCP_Container> clients)
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
				Core::fail_message(ws, Message::App::tcp_client, make_error_code(Message_Errc::ILL_FORMED));
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
		std::shared_ptr<TCP_Container> clients)
{
	Message_Data data;
	if(!parser_data(doc, data))
	{
		Core::fail_message(ws, Message::App::tcp_client, make_error_code(Message_Errc::ILL_FORMED));
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

void message_status(std::shared_ptr<TCP_Container> clients)
{
	Core::Propagator::write_all(Byte_Array(make_status_message(clients)));
}

static void message_close(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
		std::shared_ptr<TCP_Container> clients)
{
	Message_Close data;

	if(!parser_close(doc, data))
	{
		Core::fail_message(ws, Message::App::tcp_client, make_error_code(Message_Errc::ILL_FORMED));
		return;
	}

	boost::system::error_code ec;
	clients->close(data.local, ec);
	if(ec)
	{
		Core::fail_message(ws, Message::App::tcp_client, ec);
		return;
	}
}

template<typename Client>
static void init_client(Client client,
		Message_Open& data,
		std::shared_ptr<TCP_Container> clients)
{
	client->send_status = [clients](){ message_status(clients); };
	client->close_cb = [clients, client]()
	{
		boost::system::error_code ec;
		clients->close(client->local, ec);
	};

	client->keep_alive(data.options.idle,
			data.options.count,
			data.options.interval,
			data.options.keep_alive);
	message_status(clients);
}

static void message_open(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
		std::shared_ptr<TCP_Container> clients)
{
	Message_Open data;

	if(!parser_open(doc, data))
	{
		Core::fail_message(ws, Message::App::tcp_client, make_error_code(Message_Errc::ILL_FORMED));
		return;
	}

	boost::system::error_code ec;
	if(data.secure == Secure_Type::plain)
	{
		clients->open<TCP_Client<false>>(data.remote, ec,
			[clients, &data](std::shared_ptr<TCP_Client<false>> client,
					boost::system::error_code ec)
		{
			if(ec) return;
			init_client(client, data, clients);
		});
	}
	else
	{
#if USE_SSL == 1
		clients->open<TCP_Client<true>>(data.remote, ec,
					[clients, &data](std::shared_ptr<TCP_Client<true>> client,
							boost::system::error_code ec)
			{
				if(ec) return;
				init_client(client, data, clients);
			});
#else
		ec = boost::system::errc::make_error_code(boost::system::errc::wrong_protocol_type);
#endif /* USE_SSL == 1 */
	}

	if(ec)
	{
		Core::fail_message(ws, Message::App::tcp_client, ec);
		return;
	}
}

}//TCP_Client
}//Apps

