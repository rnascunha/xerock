#ifndef MY_ASYNC_TCP_SERVER_SESSION_STATIC_CALLBACK_HPP__
#define MY_ASYNC_TCP_SERVER_SESSION_STATIC_CALLBACK_HPP__

#include "session_base.hpp"
#include "../util/static_callback.hpp"

namespace My_Async{
namespace TCP{

template<bool UseSSL,
		typename InContainer,
		typename OutContainer = InContainer,
		std::size_t ReadBufferSize = 1024>
class Session_Static_Callback final :
		public Session_Base<Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>,
							UseSSL, InContainer, OutContainer, ReadBufferSize>,
		public My_Async::Util::Static_Callback<Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>, InContainer>,
		public std::enable_shared_from_this<Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>>
{
	using self_type = Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>;
	using base_type = Session_Base<Session_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>,
			UseSSL, InContainer, OutContainer, ReadBufferSize>;
	public:
		using base_type::Session_Base;
	protected:
		virtual void read_handler(InContainer data) noexcept override{
			if(self_type::on_message_f) self_type::on_message_f(this->shared_from_this(), data);
		}

		virtual void on_open() noexcept override{
			if(self_type::on_open_f) self_type::on_open_f(this->shared_from_this());
		}

		virtual void on_error(boost::system::error_code ec, char const* what) noexcept override{
			if(self_type::on_error_f) self_type::on_error_f(this->shared_from_this(), ec, what);
		}

		virtual void on_close(boost::system::error_code ec) noexcept override{
			if(self_type::on_close_f) self_type::on_close_f(this->shared_from_this(), ec);
		}
};

}//TCP
}//My_Async


#endif /* MY_ASYNC_TCP_SERVER_SESSION_STATIC_CALLBACK_HPP__ */
