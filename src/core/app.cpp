#include "core/app.hpp"

namespace Core{

App::App(Message::App const app, const char* name,
				func_app func)
: app_(app), name_(name), func_(func){}

App::App() :
	app_(Message::App::main), name_(NULL), func_(nullptr)
{}

Message::App App::app() const noexcept
{
	return app_;
}

const char* App::name() const noexcept
{
	return name_;
}

void App::exec(
		std::shared_ptr<Core::Propagator> ws,
		Message::Main_Data& main, Message::Type_Recv& msg
		) const noexcept
{
	if(func_) func_(ws, main, msg);
}

void Dispatcher::register_app(App&& app)
{
	auto app_s = apps.find(app.app());
	if(app_s != apps.end()) return;

	apps[app.app()] = app;
}

bool Dispatcher::app(App& app, Message::App const app_m)
{
	auto app_s = apps.find(app_m);
	if(app_s == apps.end()) return false;

	app = app_s->second;
	return true;
}

bool Dispatcher::name(App& app, const char* name)
{
	for(auto& a : apps){
		if(std::string(a.second.name()) == name){
			app = a.second;
			return true;
		}
	}
	return false;
}

bool Dispatcher::exec(Message::App const app_m,
		std::shared_ptr<Core::Propagator> ws,
		Message::Main_Data& main, Message::Type_Recv& msg
		)
{
	auto app_s  = apps.find(app_m);
	if(app_s == apps.end()) return false;

	app_s->second.exec(ws, main, msg);

	return true;
}

}//Core

