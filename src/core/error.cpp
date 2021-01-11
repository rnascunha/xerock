#include "core/error.hpp"

const char* Message_Err_Category::name() const noexcept
{
	return "Message";
}

std::string Message_Err_Category::message(int ev) const
{
	switch(static_cast<Message_Errc>(ev)){
		case Message_Errc::ILL_FORMED:
			return "Ill formed message";
		case Message_Errc::APP_NOT_FOUND:
			return "App not found";
		case Message_Errc::SERVER_INTERNAL_ERROR:
			return "Server internal error";
		case Message_Errc::RESOURCE_IN_USE:
			return "Resoruce in use";
		case Message_Errc::RESOURCE_NOT_FOUND:
			return "Resource not founde";
		default:
			return "(unrecognized error)";
	}
}

std::error_code make_error_code(Message_Errc e)
{
  return {static_cast<int>(e), the_Message_Err_Category};
}
