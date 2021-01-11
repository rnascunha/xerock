#ifndef SERIAL_APP_DATA_STRUCTURE_HPP__
#define SERIAL_APP_DATA_STRUCTURE_HPP__

#include "boost/asio/serial_port_base.hpp"

namespace Apps{
namespace Serial{

struct Serial_Config{
	std::string 									port;
	unsigned int									baud;
	boost::asio::serial_port_base::character_size 	char_size;
	boost::asio::serial_port_base::parity::type	 	parity;
	boost::asio::serial_port_base::stop_bits::type 	stopbits;
	boost::asio::serial_port_base::flow_control::type	fc;
	uint8_t											fc_start;
	uint8_t		 									fc_stop;
};

}//Serial
}//Apps

#endif /* SERIAL_APP_DATA_STRUCTURE_HPP__ */
