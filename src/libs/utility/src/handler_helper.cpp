#include "util/handler_helper.hpp"

#ifdef __unix__
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>

#include <iostream>
#endif /* __unix__ */

namespace Util {

using handler_t = int;

#ifdef __unix__

/**
 * Doesnt check if the same process is locking the file
 */
bool check_lock_handler(handler_t handler) noexcept
{
	if(handler <= 0) return false;

	struct flock fl = {};
	fl.l_type   = F_WRLCK;  /* read/write lock */
	fl.l_whence = SEEK_SET;

	::fcntl(handler, F_GETLK, &fl);
	if(fl.l_type == F_UNLCK) return false;
	return true;
}


/**
 * Non-BLocking
 */
void lock_handler(handler_t handler, std::error_code& ec) noexcept
{
	if(handler <= 0){
		ec = std::make_error_code(std::errc::bad_file_descriptor);
		return;
	}

	struct flock fl = {};
	fl.l_type   = F_WRLCK;  /* read/write lock */
	fl.l_whence = SEEK_SET;

	int err = ::fcntl(handler, F_SETLK, &fl);
	if(err == -1){
		ec = std::make_error_code(std::errc::no_lock_available);
	}
}

bool lock_handler(handler_t handler) noexcept
{
	std::error_code ec;

	lock_handler(handler, ec);
	if(ec) return false;

	return true;
}

void unlock_handler(handler_t handler, std::error_code& ec) noexcept
{
	if(handler <= 0){
		ec = std::make_error_code(std::errc::bad_file_descriptor);
		return;
	}

	struct flock fl = {};
	fl.l_type   = F_UNLCK;  /* unlock */
	fl.l_whence = SEEK_SET;

	int err = ::fcntl(handler, F_SETLK, &fl);
	if(err == -1){
		ec = std::make_error_code(std::errc::operation_not_permitted);
	}
}

bool unlock_handler(handler_t handler) noexcept
{
	std::error_code ec;

	unlock_handler(handler, ec);
	if(ec) return false;

	return true;
}

bool lock_handler_if_not(handler_t handler) noexcept
{
	if(! check_lock_handler(handler)){
		return lock_handler(handler);
	}
	return false;
}

#else

bool check_lock_handler(handler_t) noexcept
{
	return true;
}


/**
 * Non-BLocking
 */
void lock_handler(handler_t, std::error_code&) noexcept
{

}

bool lock_handler(handler_t) noexcept
{
	return true;
}

void unlock_handler(handler_t, std::error_code&) noexcept
{

}

bool unlock_handler(handler_t) noexcept
{
	return true;
}

bool lock_handler_if_not(handler_t) noexcept
{
	return true;
}


#endif /* __unix__ */

}//Util
