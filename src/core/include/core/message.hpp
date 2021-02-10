#ifndef MESSAGE_HEADER_HPP__
#define MESSAGE_HEADER_HPP__

#include <cstdint>

namespace Message {

typedef	std::uint64_t time_t;

enum class App{
	main = 0,
	echo,
	serial,
	monitor,
	tcp_server,
	tcp_client,
	udp_client,
};

enum class Type{
	data = 0,
	control
};

enum class Control{
	status = 0,
	config,
	open,
	close,
	custom,
	error
};

struct Main_Data{
	App 		app;
	time_t		time;
	Type 		type;
	Control		control;
};

}

#ifdef JSON_MESSAGES

#include <rapidjson/document.h>
#include <string>

namespace Message{

using Type_Recv = rapidjson::Document;
using Type_Send = std::string;

}//Message

#include "json/factory.hpp"

#else
#error Message type not defined
#endif


#endif /* MESSAGE_HEADER_HPP__ */
