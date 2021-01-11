#ifndef SERIAL_EXCEPTION_HPP__
#define SERIAL_EXCEPTION_HPP__

#include <string>
#include <exception>

namespace My_Async{
namespace Serial{

struct Serial_Exception : std::exception{
	int error;
	std::string str_error;

	Serial_Exception(int err, const std::string& str_err) : error(err) { str_error = str_err; }
	virtual const char* what() const throw() override { return str_error.c_str(); }
};

} //Serial
} //My_Async

#endif /* SERIAL_EXCEPTION_HPP__ */
