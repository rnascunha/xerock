#include "core/functions.hpp"
#include <chrono>

namespace Core{

static long userid = (server_user_id + 1);

user_id_type user_id()
{
	return userid++;
}

mid_type get_mid()
{
	static long mid = 0;

	return mid++;
}

}//Core

namespace Message{

time_type time(){
	return std::chrono::duration_cast<std::chrono::microseconds>(
									std::chrono::system_clock::now()
										.time_since_epoch())
											.count();
}

}//Message
