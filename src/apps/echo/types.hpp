#ifndef ECHO_APP_STRUCUTRE_HPP__
#define ECHO_APP_STRUCUTRE_HPP__

#include "byte_array.h"

namespace Apps{
namespace Echo{

enum class Echo_Type{
	string = 0,
	binary,
	hex,
	octo
};

struct Echo_Data{
	Echo_Type	type;
	Byte_Array	data;
};

}//Echo
}//Apps

#endif /* ECHO_APP_STRUCUTRE_HPP__ */
