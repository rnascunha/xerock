#include "init_apps.hpp"
#include "core/app.hpp"

#include "main/main.hpp"

#if USE_APP_ECHO == 1
#include "echo/echo.hpp"
#endif

#if USE_APP_SERIAL == 1
#include "serial/serial.hpp"
#endif

#if USE_APP_MONITOR == 1
#include "monitor/monitor.hpp"
#endif

#if USE_APP_TCP_SERVER > 0 && USE_APP_TCP_SERVER <= 3
#include "tcp_server/tcp_server.hpp"
#endif

#if USE_APP_TCP_CLIENT > 0 && USE_APP_TCP_CLIENT <= 3
#include "tcp_client/tcp_client.hpp"
#endif

#if USE_APP_UDP_CLIENT == 1
#include "udp_client/udp_client.hpp"
#endif

namespace Apps{

#if USE_SSL == 1
void init_apps(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx)
#else
void init_apps(boost::asio::io_context& ioc)
#endif /* USE_SSL == 1 */
{
	Core::Dispatcher::register_app(Core::App{Message::App::main, Apps::Main::name});

#if USE_APP_ECHO == 1
	Core::Dispatcher::register_app(Core::App{Message::App::echo, Apps::Echo::name, Apps::Echo::echo_app});
#endif /* USE_APP_ECHO == 1 */

#if USE_APP_SERIAL == 1
	Core::Dispatcher::register_app(Core::App{Message::App::serial, Apps::Serial::name,
					std::bind(Apps::Serial::serial_app,
							std::placeholders::_1,
							std::placeholders::_2,
							std::placeholders::_3,
							std::make_shared<Apps::Serial::Container>(ioc))}
    );
#endif /* USE_APP_SERIAL == 1	 */

#if USE_APP_MONITOR == 1
	Core::Dispatcher::register_app(Core::App{Message::App::monitor, Apps::Monitor::name,
					std::bind(Apps::Monitor::monitor_app,
							std::placeholders::_1,
							std::placeholders::_2,
							std::placeholders::_3,
							std::make_shared<Apps::Monitor::Executor>(ioc))}
	);
#endif /* USE_APP_MONITOR == 1 */

#if USE_APP_TCP_SERVER > 0 && USE_APP_TCP_SERVER <= 3
#if USE_APP_TCP_SERVER == 1
    auto tcp_servers = std::make_shared<Apps::TCP_Server::TCP_Container>(ioc);
#elif USE_APP_TCP_SERVER == 2 || USE_APP_TCP_SERVER == 3
    auto tcp_servers = std::make_shared<Apps::TCP_Server::TCP_Container>(ioc, ctx);
#endif /* USE_SSL == 1 */
	Apps::TCP_Server::init_cb(tcp_servers);
	Core::Dispatcher::register_app(Core::App{Message::App::tcp_server, Apps::TCP_Server::name,
					std::bind(Apps::TCP_Server::tpc_server_app,
							std::placeholders::_1,
							std::placeholders::_2,
							std::placeholders::_3,
							tcp_servers)}
	);

#endif /* USE_APP_TCP_SERVER > 0 && USE_APP_TCP_SERVER <= 3 */

#if USE_APP_TCP_CLIENT > 0 && USE_APP_TCP_CLIENT <= 3
#if USE_APP_TCP_CLIENT == 1
    auto tcp_clients = std::make_shared<Apps::TCP_Client::TCP_Container>(ioc);
#elif USE_APP_TCP_SERVER == 2 || USE_APP_TCP_SERVER == 3
    auto tcp_clients = std::make_shared<Apps::TCP_Client::TCP_Container>(ioc, ctx);
#endif /* USE_SSL == 1 */
//	Apps::TCP_Client::init_cb(tcp_clients);
	Core::Dispatcher::register_app(Core::App{Message::App::tcp_client, Apps::TCP_Client::name,
					std::bind(Apps::TCP_Client::tpc_client_app,
							std::placeholders::_1,
							std::placeholders::_2,
							std::placeholders::_3,
							tcp_clients)}
	);
#endif /* USE_APP_TCP_SERVER > 0 && USE_APP_TCP_SERVER <= 3 */

#if USE_APP_UDP_CLIENT == 1
	Core::Dispatcher::register_app(Core::App{Message::App::udp_client, Apps::UDP_Client::name,
					std::bind(Apps::UDP_Client::udp_client_app,
							std::placeholders::_1,
							std::placeholders::_2,
							std::placeholders::_3,
							std::make_shared<Apps::UDP_Client::UDP_Container>(ioc))}
	);
#endif /* USE_APP_UDP_CLIENT == 1 */

}

}//Apps
