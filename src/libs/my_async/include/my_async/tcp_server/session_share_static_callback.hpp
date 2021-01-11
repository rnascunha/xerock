#ifndef MY_ASYNC_TCP_SERVER_SESSION_SHARE_STATIC_CALLBACK_HPP__
#define MY_ASYNC_TCP_SERVER_SESSION_SHARE_STATIC_CALLBACK_HPP__

#include "session_base.hpp"
#include "../util/static_callback.hpp"
#include "util/shareable.hpp"

namespace My_Async{
namespace TCP{

template<bool UseSSL,
		typename InContainer,
		typename OutContainer = InContainer,
		std::size_t ReadBufferSize = 1024>
class Session_Share_Static_Callback final :
		public Session_Base<Session_Share_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>,
							UseSSL, InContainer, OutContainer, ReadBufferSize>,
		public My_Async::Util::Static_Callback<Session_Share_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>, InContainer>,
		public std::enable_shared_from_this<Session_Share_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>>
{
		using self_type = Session_Share_Static_Callback<UseSSL, InContainer, OutContainer, ReadBufferSize>;
		using base_type = Session_Base<self_type,
				UseSSL, InContainer, OutContainer, ReadBufferSize>;
	public:
		static constexpr const bool is_shareable = true;
#if USE_SSL == 1
		//SSL contructors
		explicit
		Session_Share_Static_Callback(
			boost::asio::basic_stream_socket<boost::asio::ip::tcp>&& stream,
			boost::asio::ssl::context& ctx,
			std::shared_ptr<Shareable<self_type>> share)
			: base_type(std::move(stream), ctx), share_(share){}
#endif
		//Plain contructor
		explicit
		Session_Share_Static_Callback(
			boost::asio::basic_stream_socket<boost::asio::ip::tcp>&& stream,
			std::shared_ptr<Shareable<self_type>> share)
		  : base_type(std::move(stream)), share_(share){}

		void write_all(OutContainer const data)
		{
			share_->exec_to_all(
				std::bind(
					&self_type::write_share,
					std::placeholders::_1,
					std::make_shared<OutContainer const>(std::move(data))
				)
			);
		}
	protected:
		std::shared_ptr<Shareable<self_type>> share_;

		virtual void read_handler(InContainer data) noexcept override{
			if(self_type::on_message_f) self_type::on_message_f(this->shared_from_this(), data);
		}

		virtual void on_open() noexcept override{
			share_->join(this);
			if(self_type::on_open_f) self_type::on_open_f(this->shared_from_this());
		}

		virtual void on_error(boost::system::error_code ec, char const* what) noexcept override{
			if(self_type::on_error_f) self_type::on_error_f(this->shared_from_this(), ec, what);
		}

		virtual void on_close(boost::system::error_code ec) noexcept override{
			share_->leave(this);
			if(self_type::on_close_f) self_type::on_close_f(this->shared_from_this(), ec);
		}
};

}//TCP
}//My_Async


#endif /* MY_ASYNC_TCP_SERVER_SESSION_SHARE_STATIC_CALLBACK_HPP__ */
