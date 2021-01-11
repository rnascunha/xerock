#include "arguments.hpp"

#include <sstream>
#include <iostream>

#include "argh.h"
#include "version.hpp"

std::string make_command_line_help(const char* name)
{
	std::stringstream ss;

	ss << name << " [options...] <port>";

	return ss.str();
}

std::string make_error_help(const char* name)
{
	std::stringstream ss;

	ss << "Usage:\n\t" << make_command_line_help(name) << R"(
Example:
	)" << name << R"( 8080
	)" << name << R"( --addr 127.0.0.1 --threads 5 8080
	)" << name << R"( --key domain.key --cert domain.crt 8080
More options use: -h)";

	return ss.str();
}

std::string make_help(const char* name)
{
	std::stringstream ss;

	ss << make_command_line_help(name) << R"(
Required:
	<port>: socket port.
Required when compiled with SSL support:
	-k, --key=<path_key_file>
		PEM file with the SSL private key.
	-c, --cert=<path_cert_file>
		PEM file with SSL certificate
Options:
	-h, --help
		This help message.
	-a, --addr=<addr> | Default: 0.0.0.0 (all interfaces)
		IP address that will be listened.
	-t, --threads=<num> | Default: 1
		Number of threads that will share the work. <num> must be a number equal or greater 1.
 	-s, --ssl
		Print 'true' if compiled with SSL support (otherwise 'false').
	-p, --apps
		Prints compiled apps name.
	-v, --version
		Prints version infomartion)";

	return ss.str();
}

void print_help(const char* name)
{
	std::cout << make_help(name) << "\n";
}

void check_arguments(int, char* argv[], arguments& args)
{
	argh::parser cmdl;
	cmdl.add_params({"-a", "--addr"});
	cmdl.add_params({"-k", "--key"});
	cmdl.add_params({"-c", "--cert"});
	cmdl.add_params({"-t", "--threads"});

	cmdl.parse(argv);

	if(cmdl[{"-h", "--help"}])
	{
		print_help(argv[0]);
		exit(EXIT_SUCCESS);
	}

	if(cmdl[{"-s", "-ssl"}])
	{
		std::cout << (USE_SSL ? "true" : "false") << "\n";
		exit(EXIT_SUCCESS);
	}

	if(cmdl[{"-p", "--apps"}])
	{
		std::stringstream ss;

		ss << "echo=" << (USE_APP_ECHO ? "true" : "false");
		ss << ",serial=" << (USE_APP_SERIAL ? "true" : "false");
		ss << ",monitor=" << (USE_APP_MONITOR ? "true" : "false");
		ss << ",tcp_server=" << (USE_APP_TCP_SERVER == 0 ? "false" : (
				(USE_APP_TCP_SERVER == 1 ? "plain" : (
						USE_APP_TCP_SERVER == 2 ? "ssl" : "plain,ssl"))));
		std::cout << ss.str() << "\n";

		exit(EXIT_SUCCESS);
	}

	if(cmdl[{"-v", "--version"}])
	{
		std::cout << "./xerock " << VERSION << "\n"
				<< "Copyright (C) " << COPYRIGHT_YEAR << " " << AUTHOR << "\n";
		exit(EXIT_SUCCESS);
	}

	/**
	 * Checking port
	 */
	if(cmdl.pos_args().size() != 2)
	{
		std::cerr << "ERROR! You need to provide the listening port.\n"
				<< make_error_help(argv[0]) << "\n";
		exit(EXIT_FAILURE);
	}

	if(!(cmdl(1) >> args.port) || !(args.port > 1 && args.port <= 65535))
	{
		std::cerr << "ERROR! Listening port must be a positive number.\n"
				<< make_error_help(argv[0]) << "\n";

		exit(EXIT_FAILURE);
	}

	/**
	 * Address
	 */
	cmdl({"-a", "--addr"}, "0.0.0.0") >> args.addr;

	if(!(cmdl({"-t", "--threads"}, 1) >> args.threads) || args.threads < 1)
	{
		std::cerr << "ERROR! -t/--threads: argument option must be a positive interger (>= 1)\n";
		exit(EXIT_FAILURE);
	}

#if USE_SSL == 1
	if(!(cmdl({"-k", "--key"}) >> args.key) || !(cmdl({"-c", "--cert"}) >> args.cert))
	{
		std::cerr << "ERROR! SSL support require key(-k)/certificate(-c) files.\n"
				<< make_error_help(argv[0]) << "\n";
		exit(EXIT_FAILURE);
	}
#else /* USE_SSL == 1 */
	if(cmdl({"-k", "--key"}))
		std::cerr << "WARNING! No SSL support. Ignoring -k/--key option\n";

	if(cmdl({"-c", "--cert"}))
		std::cerr << "WARNING! No SSL support. Ignoring -c/--cert option\n";
#endif /* USE_SSL == 1 */
}

void print_args(arguments const& args)
{
	std::cout << "--addr=" << args.addr << "\n"
			<< "--threads=" << args.threads << "\n"
#if USE_SSL == 1
			<< "--key=" << args.key << "\n"
			<< "--cert=" << args.cert << "\n"
#endif
			<< "Port=" << args.port << "\n";
}

