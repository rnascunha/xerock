#include "core/websocket_session.hpp"

#include <memory>

#include "core/app.hpp"
#include "core/helpers.hpp"
#include "core/error.hpp"
#include "core/functions.hpp"
#include "core/json/factory.hpp"

#include "../../apps/main/make.hpp"
#include "../../apps/main/parser.hpp"

#include "util/json_helper.hpp"
//#include <iostream>

namespace Core{

static void on_open(std::shared_ptr<Propagator>) noexcept;
static void on_close(std::shared_ptr<Propagator>, boost::system::error_code) noexcept;
static void on_message(std::shared_ptr<Propagator>,
		Byte_Array const&) noexcept;

static void fail(std::shared_ptr<Propagator>,
		boost::system::error_code ec,
		const char* what)
{
	std::cerr << what << ": " << ec.message() << "\n";
}

void init_websocket_cb()
{
	Propagator::set_on_open(on_open);
	Propagator::set_on_close(on_close);
	Propagator::set_on_message(std::bind(
									on_message,
									std::placeholders::_1,
									std::placeholders::_2));
	Propagator::set_on_error(fail);
}

static void send_status(unsigned int nusers);
static void send_uid(std::shared_ptr<Propagator> ws);
static void send_app_list(std::shared_ptr<Propagator> ws);

void on_open(std::shared_ptr<Propagator> ws) noexcept
{
	ws->text(true);
	send_uid(ws);
	send_app_list(ws);
	send_status(static_cast<unsigned int>(Propagator::get_num_shares()));
}

void on_close(std::shared_ptr<Propagator>, boost::system::error_code) noexcept
{
	send_status(static_cast<unsigned int>(Propagator::get_num_shares()) - 1);
}

void on_message(std::shared_ptr<Propagator> ws,
		Byte_Array const& body) noexcept
{
	rapidjson::Document d;

	std::string json(body.to_string());
	if (d.Parse(json.c_str()).HasParseError() || !d.IsObject())
	{
		fail_message(ws, Message::App::main, make_error_code(Message_Errc::ILL_FORMED));
		return;
	}

	//Echoing to the other users (and... same user)
	Propagator::write_all(Byte_Array(std::move(Message::Factory::add_mid<true>(d))), true);

	Message::Main_Data main;
	if(!Apps::Main::parser_main(d, main))
	{
		fail_message(ws, Message::App::main, make_error_code(Message_Errc::ILL_FORMED));
		return;
	}
	Util::print_json(d);

	if(!Dispatcher::exec(main.app, ws, main, d))
	{
		return fail_message(ws, Message::App::main, make_error_code(Message_Errc::APP_NOT_FOUND));
	}
}

/**
 *
 *
 *
 *
 *
 */

static void send_status(unsigned int nusers)
{
	Propagator::write_all(std::move(Apps::Main::make_status(nusers)));
}

static void send_uid(std::shared_ptr<Propagator> ws)
{
	ws->write(std::move(Apps::Main::make_uid()));
}

static void send_app_list(std::shared_ptr<Propagator> ws)
{
	std::vector<std::string> app_str;
	Dispatcher::for_each([&app_str](App const& a){
		app_str.push_back(a.name());
	});

	ws->write(Byte_Array(Apps::Main::make_app_list(app_str)));
}

}//Core
