#ifndef MAIN_CORE_DISPATCHER_APP_HPP__
#define MAIN_CORE_DISPATCHER_APP_HPP__

#include <unordered_map>
#include <functional>

#include "types.hpp"
#include "message.hpp"

namespace Core{

class Dispatcher
{
	public:
		Dispatcher();

		void register_app(App&&);

		bool app(App&, Message::App const);
		bool name(App&, const char*);
		bool exec(Message::App const,
				std::shared_ptr<Core::Propagator>,
				Message::Main_Data&, Message::Type_Recv&
				);

		template<class Func>
		void for_each(Func f){
			std::for_each(cbegin(apps), cend(apps), [&f](const std::pair<const Message::App, App>& a){
				f(a.second);
			});
		}
	protected:
		inline std::unordered_map<Message::App, App> apps;
};

}

#endif /* MAIN_CORE_DISPATCHER_APP_HPP__ */
