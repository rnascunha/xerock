#ifndef ARGUMENTS_PARSE_HPP__
#define ARGUMENTS_PARSE_HPP__

#include <string>

#define DEFAULT_ADDRESS		"0.0.0.0"
#define INVALID_PORT		0
#define DEFAULT_NUM_THREADS	1

struct arguments {
	std::string addr = DEFAULT_ADDRESS;
	int port = INVALID_PORT;
	int threads = DEFAULT_NUM_THREADS;
#if USE_SSL == 1
	std::string key;
	std::string cert;
#endif /* USE_SSL == 1 */
};

std::string make_command_line_help(const char* name);
std::string make_help(const char* name);
std::string make_error_help(const char* name);

void print_help(const char* name);

void check_arguments(int argc, char* argv[], arguments&);
void print_args(arguments const& args);

#endif /* ARGUMENTS_PARSE_HPP__ */
