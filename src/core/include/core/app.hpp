#ifndef MAIN_APP_APPS_HPP__
#define MAIN_APP_APPS_HPP__

#include <unordered_map>
#include <functional>

#include "types.hpp"
#include "message.hpp"

namespace Core{

using func_app = std::function<void(
		std::shared_ptr<Core::Propagator>,
		Message::Main_Data&, Message::Type_Recv&
		)>;

class App{
	public:
		App(Message::App const app, const char* name,
				func_app func = nullptr);

		App();

		Message::App app() const noexcept;
		const char* name() const noexcept;
		void exec(
				std::shared_ptr<Core::Propagator>,
				Message::Main_Data&, Message::Type_Recv&
				) const noexcept;

		App& operator=(const App& a) = default;
	protected:
		Message::App app_;
		const char* name_;
		func_app func_;
};

class Dispatcher{
	public:
		static void register_app(App&&);

		static bool app(App&, Message::App const);
		static bool name(App&, const char*);
		static bool exec(Message::App const,
				std::shared_ptr<Core::Propagator>,
				Message::Main_Data&, Message::Type_Recv&
				);

		template<class Func>
		static void for_each(Func f){
			std::for_each(cbegin(apps), cend(apps), [&f](const std::pair<const Message::App, App>& a){
				f(a.second);
			});
		}
	protected:
		static inline std::unordered_map<Message::App, App> apps;
};

}//Core

#endif /* MAIN_APP_APPS_HPP__ */
