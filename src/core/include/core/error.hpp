#ifndef MESSAGE_ERROR_CODE_HPP__
#define MESSAGE_ERROR_CODE_HPP__

#include <system_error>

enum class Message_Errc{
	ILL_FORMED = 10,
	APP_NOT_FOUND = 11,
	SERVER_INTERNAL_ERROR = 20,
	RESOURCE_IN_USE = 21,
	RESOURCE_NOT_FOUND = 22,
};

struct Message_Err_Category : public std::error_category{
	const char* name() const noexcept;
	std::string message(int ev) const;
};

const Message_Err_Category the_Message_Err_Category {};

std::error_code make_error_code(Message_Errc e);

#endif /* MESSAGE_ERROR_CODE_HPP__ */
