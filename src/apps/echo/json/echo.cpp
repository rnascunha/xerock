#include "../echo.hpp"
#include "../parser.hpp"

#include "core/helpers.hpp"
#include "core/error.hpp"
#include "core/types.hpp"

namespace Apps{
namespace Echo{

static void send_echo(std::shared_ptr<Core::Propagator>,
					rapidjson::Document&);

void echo_app(std::shared_ptr<Core::Propagator> ws,
		Message::Main_Data&,
		rapidjson::Document& doc)
{
	Echo_Data data;
	if(!parser_echo(doc, data))
		return Core::fail_message(ws, Message::App::echo, make_error_code(Message_Errc::ILL_FORMED));

	send_echo(ws, doc);
}

static void send_echo(std::shared_ptr<Core::Propagator>,
					rapidjson::Document& doc){

	auto const message = Message::Factory::create(Message::App::echo,
											doc[DATA_KEY]);

	Core::Propagator::write_all(std::move(message), true);
}

}//Echo
}//Apps

