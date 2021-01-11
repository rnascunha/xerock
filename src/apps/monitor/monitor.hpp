#ifndef MONITOR_APP_HPP__
#define MONITOR_APP_HPP__

#include "core/message.hpp"
#include "executor.hpp"

#include "boost/asio.hpp"

namespace Apps{
namespace Monitor{

static constexpr const char* name = "monitor";

void monitor_app(std::shared_ptr<Core::Propagator>,
		const Message::Main_Data&, const Message::Type_Recv&,
		std::shared_ptr<Executor> monitor);

}//Monitor
}//Apps

#endif /* MONITOR_APP_HPP__ */
