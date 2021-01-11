#ifndef HANDLER_HELPERS_HPP__
#define HANDLER_HELPERS_HPP__

#include <system_error>

namespace Util{

using handler_t = int;

bool check_lock_handler(handler_t handler) noexcept;

void lock_handler(handler_t handler, std::error_code& ec) noexcept;
bool lock_handler(handler_t handler) noexcept;

void unlock_handler(handler_t handler, std::error_code& ec) noexcept;
bool unlock_handler(handler_t handler) noexcept;

bool lock_handler_if_not(handler_t handler) noexcept;

}//Util

#endif /* HANDLER_HELPERS_HPP__ */
