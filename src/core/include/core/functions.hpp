#ifndef CORE_FUNCTIONS_HPP__
#define CORE_FUNCTIONS_HPP__

#include <cstdint>
#include "types.hpp"

namespace Core{

static constexpr const user_id_type server_user_id = 0;

mid_type get_mid();
user_id_type user_id();

}//Core

namespace Message{
time_type time();
}

#endif /* CORE_FUNCTIONS_HPP__ */
