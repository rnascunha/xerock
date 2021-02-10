#ifndef UDP_CLIENT_CALLBACK_SESSION_HPP__
#define UDP_CLIENT_CALLBACK_SESSION_HPP__

#include "client_base.hpp"
#include <memory>
#include <functional>

namespace My_Async{
namespace UDP{

template<typename InContainer,
		typename OutContainer = InContainer,
		std::size_t ReadBufferSize = 1024>
class Client_Callback final : public Client_Base<Client_Callback<InContainer, OutContainer, ReadBufferSize>, InContainer, OutContainer, ReadBufferSize>,
								public std::enable_shared_from_this<Client_Callback<InContainer, OutContainer, ReadBufferSize>>
{
	using self_type = Client_Callback<InContainer, OutContainer, ReadBufferSize>;
	using base_type = Client_Base<Client_Callback<InContainer, OutContainer, ReadBufferSize>, InContainer, OutContainer, ReadBufferSize>;

	public:
		using base_type::Client_Base;

		using on_message_t = std::function<void(std::shared_ptr<self_type>, InContainer const&)>;
		using on_error_t = std::function<void(std::shared_ptr<self_type>, boost::system::error_code, char const*)>;

		void set_on_message(on_message_t func = nullptr) noexcept
		{
			on_message_f = func;
		}

		void set_on_error(on_error_t func = nullptr) noexcept
		{
			on_error_f = func;
		}

	protected:
		void read_handler(InContainer data) noexcept override
		{
			if(this->on_message_f) this->on_message_f(this->shared_from_this(), data);
		}

		void fail(boost::system::error_code ec, char const* what) noexcept override
		{
			if(this->on_error_f) this->on_error_f(this->shared_from_this(), ec, what);
		}

		on_message_t on_message_f;
		on_error_t on_error_f;
};

}//UDP
}//My_Async

#endif /* UDP_CLIENT_CALLBACK_SESSION_HPP__ */
