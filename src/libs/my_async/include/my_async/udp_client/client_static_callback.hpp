#ifndef UDP_CLIENT_STATIC_CALLBACK_SESSION_HPP__
#define UDP_CLIENT_STATIC_CALLBACK_SESSION_HPP__

#include "client_base.hpp"
#include <memory>
#include <functional>
#include "../util/callback.hpp"

namespace My_Async{
namespace UDP{

template<typename InContainer,
		typename OutContainer = InContainer,
		std::size_t ReadBufferSize = 1024>
class Client_Static_Callback final : public Client_Base<Client_Static_Callback<InContainer, OutContainer, ReadBufferSize>, InContainer, OutContainer, ReadBufferSize>,
								public My_Async::Util::Static_Callback<Client_Static_Callback<InContainer, OutContainer, ReadBufferSize>, InContainer>,
								public std::enable_shared_from_this<Client_Static_Callback<InContainer, OutContainer, ReadBufferSize>>
{
	using self_type = Client_Static_Callback<InContainer, OutContainer, ReadBufferSize>;
	using base_type = Client_Base<Client_Static_Callback<InContainer, OutContainer, ReadBufferSize>, InContainer, OutContainer, ReadBufferSize>;

	public:
		using base_type::Client_Base;

	protected:
		void read_handler(InContainer data) noexcept override
		{
			if(self_type::on_message_f) self_type::on_message_f(this->shared_from_this(), data);
		}

		void on_open() noexcept override
		{
			if(self_type::on_open_f) self_type::on_open_f(this->shared_from_this());
		}

		void on_error(boost::system::error_code ec, char const* what) noexcept override
		{
			if(self_type::on_error_f) self_type::on_error_f(this->shared_from_this(), ec, what);
		}

		void on_close(boost::system::error_code ec) noexcept override
		{
			if(self_type::on_close_f) self_type::on_close_f(this->shared_from_this(), ec);
		}
};

}//UDP
}//My_Async

#endif /* UDP_CLIENT_STATIC_CALLBACK_SESSION_HPP__ */
