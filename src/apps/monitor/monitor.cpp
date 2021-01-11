#include "monitor.hpp"
#include "parser.hpp"

#include "executor.hpp"

#include "core/helpers.hpp"
#include "core/error.hpp"

namespace Apps{
namespace Monitor{

static void send_monitor_data(std::shared_ptr<Core::Propagator>,
								const Message::Type_Recv&,
								std::shared_ptr<Executor>);

void monitor_app(std::shared_ptr<Core::Propagator> ws,
		const Message::Main_Data& main, const Message::Type_Recv& msg,
		std::shared_ptr<Executor> monitor)
{
	if(main.type == Message::Type::control){
		//control
		switch(main.control){
			case Message::Control::status:
				send_monitor_data(ws, msg, monitor);
				break;
			default:
				return;
		}
	}
}

static void send_monitor_data(std::shared_ptr<Core::Propagator> ws,
								const Message::Type_Recv& doc,
								std::shared_ptr<Executor> monitor)
{
	Message_Monitor_Data data;
	if(!parse_app(doc, data))
	{
		Core::fail_message(ws, Message::App::monitor, make_error_code(Message_Errc::ILL_FORMED));
		return;
	}

	monitor->continuos(data.cont);

	if(data.type == Monitor_Type::set){
		monitor->sample(data.sample_time);
		monitor->init();
		return;
	}

	monitor->cancel();
}

}//Monitor
}//Apps

