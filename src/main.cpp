#include <boost/asio/signal_set.hpp>
#include <iostream>
#include <vector>

#include "apps/init_apps.hpp"
#include "core/types.hpp"
#include "core/websocket_session.hpp"

#if USE_SSL == 1
#include "load_certificate.hpp"
#endif /* USE_SSL == 1 */

#include "arguments.hpp"

static int fail_program(const char* program, const char* = "")
{
	std::cerr << make_error_help(program) << "\n";

	return EXIT_FAILURE;
}

int
main(int argc, char* argv[])
{
	arguments args;

	check_arguments(argc, argv, args);

    boost::system::error_code ec;
	auto const address = boost::asio::ip::make_address(args.addr, ec);
	if(ec)
	{
		std::cerr << "ERROR! [" << args.addr << "] Invalid address\n";
		return fail_program(argv[0]);
	}
	auto port = static_cast<unsigned short>(args.port);
	auto const threads = std::max<int>(1, args.threads);

	// The io_context is required for all I/O
    boost::asio::io_context ioc;

    Core::init_websocket_cb();

#if USE_SSL == 1
    boost::asio::ssl::context ctx{boost::asio::ssl::context::tlsv12};

    std::cout << "Loading SSL parameters:\n\tkey: "
    		<< args.key << "\n\tcertificate: "
    		<< args.cert << "\n";
    load_certificate(ctx, args.cert, args.key, ec);
    if(ec)
    {
    	std::cerr << "ERROR! [" << ec.value() << ": " << ec.message() << "] Invalid SSL key/certificate\n";
       	return EXIT_FAILURE;
    }
    std::cout << "SSL configuration loaded\n";

    std::cout << "Opening socket wss://" << address << ":" << port << " ...\n";
    My_Async::Listener::make_listener<Core::Propagator>(ioc, ctx, address, port, ec);

    Apps::init_apps(ioc, ctx);
#else
    std::cout << "Opening socket ws://" << address << ":" << port << " ...\n";
    My_Async::Listener::make_listener<Core::Propagator>(ioc, address, port, ec);

    Apps::init_apps(ioc);
#endif /* USE_SSL == 1 */
    if(ec)
    	return fail_program(argv[0], ec.message().c_str());

    std::cout << "Socket opened\n";

    // Capture SIGINT and SIGTERM to perform a clean shutdown
    boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(
        [&ioc](boost::system::error_code const&, int){
            ioc.stop();
        });

    std::cout << "Initiating service threads [" << args.threads << "] ...\n";
    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for(auto i = threads - 1; i > 0; --i)
        v.emplace_back(
        [&ioc]
        {
            ioc.run();
        });
    ioc.run();

    // Block until all the threads exit
    for(auto& t : v)
        t.join();

    return EXIT_SUCCESS;
}
