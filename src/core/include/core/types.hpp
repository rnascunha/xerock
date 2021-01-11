#ifndef MESSAGE_DEFINES_HPP__
#define MESSAGE_DEFINES_HPP__

#include "my_async.hpp"
#include "byte_array.h"

namespace Core{

using mid_type = uint64_t;
using user_id_type = uint32_t;

#if USE_SSL == 1
using Propagator = My_Async::Websocket::
							Session_Static_Callback_Static_Share<true, Byte_Array>;
#else
using Propagator = My_Async::Websocket::
							Session_Static_Callback_Static_Share<false, Byte_Array>;
#endif /* USE_SSL == 1 */

}//Core

namespace Message {
	using time_type = uint64_t;
}//Message

#endif /* MESSAGE_DEFINES_HPP__ */
