#ifndef TCP_SERVER_APP_TYPE_DEF_HPP__
#define TCP_SERVER_APP_TYPE_DEF_HPP__

#include "boost/asio.hpp"

#include "core/helpers.hpp"
#include "my_async.hpp"
#include "util/shareable.hpp"
#include "byte_array.h"

#include "share_data.hpp"

namespace Apps{
namespace TCP_Server{

template<bool UseSSL>
class Session_TCP_Server final :
		public My_Async::TCP::Session_Base<Session_TCP_Server<UseSSL>, UseSSL, Byte_Array>,
		public My_Async::Util::Static_Callback<Session_TCP_Server<UseSSL>, Byte_Array>,
		public std::enable_shared_from_this<Session_TCP_Server<UseSSL>>
{
		using self_type = Session_TCP_Server<UseSSL>;
		using base_type = My_Async::TCP::Session_Base<Session_TCP_Server<UseSSL>,
				UseSSL, Byte_Array>;
	public:
		static constexpr const bool is_shareable = true;
		using shareable_data_type = Data_Share;
#if USE_SSL == 1
		//SSL contructors
		explicit
		Session_TCP_Server(
			boost::asio::basic_stream_socket<boost::asio::ip::tcp>&& stream,
			boost::asio::ssl::context& ctx,
			std::shared_ptr<Shareable<self_type, shareable_data_type>> share)
			: base_type(std::move(stream), ctx), share_(share){}
#endif
		//Plain contructor
		explicit
		Session_TCP_Server(
			boost::asio::basic_stream_socket<boost::asio::ip::tcp>&& stream,
			std::shared_ptr<Shareable<self_type, shareable_data_type>> share)
		  : base_type(std::move(stream)), share_(share){}

		void write_all(Byte_Array const data)
		{
			share_->exec_to_all(
				std::bind(
					&self_type::write_share,
					std::placeholders::_1,
					std::make_shared<Byte_Array const>(std::move(data))
				)
			);
		}
	protected:
		std::shared_ptr<Shareable<self_type, shareable_data_type>> share_;

		void read_handler(Byte_Array data) noexcept override
		{
			share_->data.read_handler(data, this->endpoint(), this->local_endpoint());
		}

		void on_open() noexcept override
		{
			share_->join(this);

			if(share_->data.keepalive)
				this->keep_alive(share_->data.idle,
						share_->data.interval,
						share_->data.count);
			share_->data.send_status();
		}

		void on_error(boost::system::error_code ec, char const* what) noexcept override
		{
			std::cerr << what << ": " << ec.message() << "\n";
			Core::fail_message(Message::App::tcp_server, ec, what);
		}

		void on_close(boost::system::error_code) noexcept override
		{
			share_->leave(this);
			share_->data.send_status();
		}
};

}//TCP_Server
}//Apps


#endif /* TCP_SERVER_APP_TYPE_DEF_HPP__ */
