#include "tcp_server.hpp"
#include "parser.hpp"

#include "make.hpp"

#include "core/helpers.hpp"
#include "core/error.hpp"

#include "my_async.hpp"

namespace Apps{
namespace TCP_Server{


static void message_data(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
		std::shared_ptr<TCP_Container> servers);
static void message_close(std::shared_ptr<Core::Propagator> ws, Message::Type_Recv const& doc,
		std::shared_ptr<TCP_Container> servers);
static void message_open(std::shared_ptr<Core::Propagator> ws, Message::Type_Recv const& doc,
		std::shared_ptr<TCP_Container> servers);
static void message_close_clients(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
									std::shared_ptr<TCP_Container> servers);

void message_status(std::shared_ptr<TCP_Container> servers);

void tpc_server_app(std::shared_ptr<Core::Propagator> ws,
		Message::Main_Data const& main,
		Message::Type_Recv const& doc,
		std::shared_ptr<TCP_Container> servers)
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
			case Message::Control::custom:
				message_close_clients(ws, doc, servers);
				break;
			default:
				Core::fail_message(ws, Message::App::tcp_server, make_error_code(Message_Errc::ILL_FORMED));
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
		std::shared_ptr<TCP_Container> servers)
{
	Message_Data data;
	if(!parser_data(doc, data)){
		Core::fail_message(ws, Message::App::tcp_server, make_error_code(Message_Errc::ILL_FORMED));
		return ;
	}

	boost::system::error_code ec;
	servers->write(data.from, data.to, data.data, ec);
	if(ec){
		Core::fail_message(ws, Message::App::tcp_server, ec);
		return;
	}
}

void message_status(std::shared_ptr<TCP_Container> servers)
{
	Core::Propagator::write_all(Byte_Array(make_status_message(servers)), true);
}

static void message_close(std::shared_ptr<Core::Propagator> ws, Message::Type_Recv const& doc,
		std::shared_ptr<TCP_Container> servers)
{
	Message_Close data;

	if(!parser_close(doc, data)){
		Core::fail_message(ws, Message::App::tcp_server, make_error_code(Message_Errc::ILL_FORMED));
		return;
	}

	if(!servers->is_open(data.endpoint)){
		Core::fail_message(ws, Message::App::tcp_server, make_error_code(Message_Errc::RESOURCE_NOT_FOUND));
		return;
	}

	boost::system::error_code ec;
	servers->close(data.endpoint, ec);
	if(ec){
		Core::fail_message(ws, Message::App::tcp_server, make_error_code(Message_Errc::SERVER_INTERNAL_ERROR));
		return;
	}

	message_status(servers);
}

template<typename T>
static void set_share_data(T share,
		Message_Open& data,
		std::shared_ptr<TCP_Container> servers)
{
	share->data.send_status = [servers](){ message_status(servers); };

	share->data.keepalive = data.options.keep_alive;
	share->data.idle = data.options.idle;
	share->data.interval = data.options.interval;
	share->data.count = data.options.count;
}

static void message_open(std::shared_ptr<Core::Propagator> ws, Message::Type_Recv const& doc,
		std::shared_ptr<TCP_Container> servers)
{
	Message_Open data;

	if(!parser_open(doc, data))
	{
		Core::fail_message(ws, Message::App::tcp_server, make_error_code(Message_Errc::ILL_FORMED));
		return;
	}

	auto const addr = boost::asio::ip::make_address(data.addr);
	boost::asio::ip::tcp::endpoint endpoint(addr, data.port);

	if(servers->is_open(endpoint))
	{
		Core::fail_message(ws, Message::App::tcp_server, make_error_code(Message_Errc::RESOURCE_IN_USE));
		return;
	}

	boost::system::error_code ec;
	if(data.secure == Secure_Type::plain)
		servers->open<Session<false>>(endpoint, ec,
			[servers, &data](boost::asio::io_context& ioc,
						boost::asio::ip::tcp::endpoint ep,
						boost::system::error_code& ec)
						{
		auto listener = My_Async::Listener::make_listener<Session<false>>(ioc, ep, ec);
		set_share_data(listener->get_share(), data, servers);

		return listener;
	});
	else
#if USE_SSL == 1
		servers->open<Session<true>>(endpoint, ec,
			[servers, &data](boost::asio::io_context& ioc,
				boost::asio::ssl::context& ctx,
				boost::asio::ip::tcp::endpoint ep,
				boost::system::error_code& ec)
				{
		auto listener = My_Async::Listener::make_listener<Session<true>>(ioc, ctx, ep, ec);
		set_share_data(listener->get_share(), data, servers);

		return listener;
	});
#else
	ec = boost::system::errc::make_error_code(boost::system::errc::wrong_protocol_type);
#endif /* USE_SSL == 1 */
	if(ec)
	{
		Core::fail_message(ws, Message::App::tcp_server, ec);
		return;
	}

	message_status(servers);
}

static void message_close_clients(std::shared_ptr<Core::Propagator> ws,
		Message::Type_Recv const& doc,
									std::shared_ptr<TCP_Container> servers)
{
	Message_Close_Client data;
	if(!parser_close_clients(doc, data)){
		Core::fail_message(ws, Message::App::tcp_server, make_error_code(Message_Errc::ILL_FORMED));
		return ;
	}

	boost::system::error_code ec;
	servers->close_clients(data.endpoint, data.clients, ec);
	if(ec){
		Core::fail_message(ws, Message::App::tcp_server, ec);
		return;
	}
}

}//TCP_Server
}//Apps
