#ifndef MY_ASYNC_TCP_CLIENT_STATIC_CALLBACK_HPP__
#define MY_ASYNC_TCP_CLIENT_STATIC_CALLBACK_HPP__

#include "client_base.hpp"
#include "../util/callback.hpp"

namespace My_Async{
namespace TCP{

template<bool UseSSL,
		typename InContainer,
		typename OutContainer = InContainer,
		std::size_t ReadBufferSize = 1024>
class Client_Static_Callback final : public Client_Base<Client_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>,
												UseSSL, InContainer, OutContainer, ReadBufferSize>,
					public My_Async::Util::Static_Callback<Client_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>, InContainer>,
					public std::enable_shared_from_this<Client_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>>
{
	using self_type = Client_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>;
	using base_type = Client_Base<self_type, UseSSL, InContainer, OutContainer, ReadBufferSize>;
	using shared_type = My_Async::Util::Static_Callback<self_type, InContainer>;
	public:
		using base_type::Client_Base;

		void read_handler(InContainer data) noexcept override
		{
			if(shared_type::on_message_f) shared_type::on_message_f(this->shared_from_this(), data);
		}

		void on_open() noexcept override
		{
			if(shared_type::on_open_f) shared_type::on_open_f(this->shared_from_this());
		}

		void on_error(boost::system::error_code ec, char const* what) noexcept override
		{
			if(shared_type::on_error_f) shared_type::on_error_f(this->shared_from_this(), ec, what);
		}

		void on_close(boost::system::error_code ec) noexcept override
		{
			if(shared_type::on_close_f) shared_type::on_close_f(this->shared_from_this(), ec);
		}
};

}//TCP
}//My_Async


#endif /* MY_ASYNC_TCP_CLIENT_STATIC_CALLBACK_HPP__ */
