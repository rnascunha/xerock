#ifndef UDP_CLIENT_STATIC_CALLBACK_SESSION_HPP__
#define UDP_CLIENT_STATIC_CALLBACK_SESSION_HPP__

#include "client_base.hpp"
#include <memory>
#include <functional>

namespace My_Async{
namespace UDP{

template<typename InContainer,
		typename OutContainer = InContainer,
		std::size_t ReadBufferSize = 1024>
class Client_Static_Callback final : public Client_Base<Client_Static_Callback<InContainer, OutContainer, ReadBufferSize>, InContainer, OutContainer, ReadBufferSize>,
								public std::enable_shared_from_this<Client_Static_Callback<InContainer, OutContainer, ReadBufferSize>>
{
	using self_type = Client_Static_Callback<InContainer, OutContainer, ReadBufferSize>;
	using base_type = Client_Base<Client_Static_Callback<InContainer, OutContainer, ReadBufferSize>, InContainer, OutContainer, ReadBufferSize>;

	public:
		using base_type::Client_Base;

		using on_message_t = std::function<void(std::shared_ptr<self_type>, InContainer const&)>;
		using on_error_t = std::function<void(std::shared_ptr<self_type>, boost::system::error_code, char const*)>;

		static void set_on_message(on_message_t func = nullptr) noexcept
		{
			self_type::on_message_f = func;
		}

		static void set_on_error(on_error_t func = nullptr) noexcept
		{
			self_type::on_error_f = func;
		}

	protected:
		void read_handler(InContainer data) noexcept override
		{
			if(self_type::on_message_f) self_type::on_message_f(this->shared_from_this(), data);
		}

		void fail(boost::system::error_code ec, char const* what) noexcept override
		{
			if(self_type::on_error_f) self_type::on_error_f(this->shared_from_this(), ec, what);
		}

		static on_message_t on_message_f;
		static on_error_t on_error_f;
};

template<typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
typename Client_Static_Callback<InContainer, OutContainer, ReadBufferSize>::on_message_t
		Client_Static_Callback<InContainer, OutContainer, ReadBufferSize>::on_message_f;

template<typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
typename Client_Static_Callback<InContainer, OutContainer, ReadBufferSize>::on_error_t
		Client_Static_Callback<InContainer, OutContainer, ReadBufferSize>::on_error_f;

}//UDP
}//My_Async

#endif /* UDP_CLIENT_STATIC_CALLBACK_SESSION_HPP__ */
