#include "arguments.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

#include "argh.h"
#include "version.hpp"

static constexpr const char* attr_address = "ADDRESS";
static constexpr const char* attr_port = "PORT";
static constexpr const char* attr_threads = "THREADS";
static constexpr const char* attr_key = "KEY";
static constexpr const char* attr_cert = "CERT";

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
	-o, --config [config_file]
		Read/write config file. If no argument provided, prints the configuration passed at command line (to be redirected to a file). If a argument is provided, reads configuration from this file (inline attributes overwrite file attributes).  
 	-s, --ssl
		Print 'true' if compiled with SSL support (otherwise 'false').
	-p, --apps
		Prints compiled apps name.
	-v, --version
		Prints version infomartion)";

	return ss.str();
}

std::string make_config_options_output(const arguments& args)
{
	std::stringstream ss;

	ss << attr_port << "=" << args.port << "\n"
		<< attr_address << "=" << args.addr << "\n"
		<< attr_threads << "=" << args.threads << "\n"
#if USE_SSL == 1
		<< attr_key << "=" << args.key << "\n"
		<< attr_cert << "=" << args.cert << "\n"
#endif /* USE_SSL == 1 */
	;

	return ss.str();
}

void read_config_file(std::string const& file_config, arguments& args, const char* app_name)
{
	std::ifstream in(file_config);
	if(!in.is_open())
	{
		std::cerr << "ERROR! Configuration file \"" << file_config << "\" not found\n"
						<< make_error_help(app_name) << "\n";
		exit(EXIT_FAILURE);
	}

	std::string attr;
	while(!in.eof())
	{
		std::getline(in, attr, '=');

		if(attr.length())
		{
			if(attr == attr_port)
				in >> args.port;
			else if(attr == attr_address)
				in >> args.addr;
			else if(attr == attr_threads)
				in >> args.threads;
#if USE_SSL == 1
			else if(attr == attr_key)
				in >> args.key;
			else if(attr == attr_cert)
				in >> args.cert;
#endif /* USE_SSL == 1 */
			in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		}
	}

	in.close();
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
	cmdl.add_params({"-o", "--config"});

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
		ss << ",tcp_client=" << (USE_APP_TCP_CLIENT == 0 ? "false" : (
						(USE_APP_TCP_CLIENT == 1 ? "plain" : (
								USE_APP_TCP_CLIENT == 2 ? "ssl" : "plain,ssl"))));
		ss << ",udp_client" << (USE_APP_UDP_CLIENT ? "true" : "false");
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
	 * Read config from file
	 */
	std::string file_config;
	if(cmdl({"-o", "--config"}) >> file_config)
		read_config_file(file_config, args, argv[0]);

	/**
	 * Checking port
	 */
	if(cmdl.pos_args().size() == 2 && !(cmdl(1) >> args.port))
	{
		std::cerr << "ERROR! Invalid listening port.\n"
					<< make_error_help(argv[0]) << "\n";
		exit(EXIT_FAILURE);
	}
	else if(args.port == INVALID_PORT)
	{
		std::cerr << "ERROR! You must provide a listening port.\n"
					<< make_error_help(argv[0]) << "\n";
		exit(EXIT_FAILURE);
	}

	if(!(args.port > 1 && args.port <= 65535))
	{
		std::cerr << "ERROR! Listening port must be a positive number.\n"
				<< make_error_help(argv[0]) << "\n";

		exit(EXIT_FAILURE);
	}

	/**
	 * Address
	 */
	cmdl({"-a", "--addr"}, args.addr) >> args.addr;

	if(!(cmdl({"-t", "--threads"}, args.threads) >> args.threads) || args.threads < 1)
	{
		std::cerr << "ERROR! -t/--threads: argument option must be a positive interger (>= 1)\n";
		exit(EXIT_FAILURE);
	}

#if USE_SSL == 1
	if(cmdl({"-k", "--key"}))
		cmdl({"-k", "--key"}) >> args.key;
	if(cmdl({"-c", "--cert"}))
		cmdl({"-c", "--cert"}) >> args.cert;

	if(!args.key.length() || !args.cert.length())
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

	/**
	 * Output config
	 */
	if(cmdl[{"-o", "--config"}])
	{
		std::cout << make_config_options_output(args);
		exit(EXIT_SUCCESS);
	}
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

