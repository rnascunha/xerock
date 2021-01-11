#ifndef WEBSOCKET_SESSION_STATIC_SHARED_STATIC_CALLBACK_HPP__
#define WEBSOCKET_SESSION_STATIC_SHARED_STATIC_CALLBACK_HPP__

#include <memory>
#include "session_base.hpp"
#include "util/static_shareable.hpp"

namespace My_Async{
namespace Websocket{

template<bool UseSSL,
		typename InContainer,
		typename OutContainer = InContainer,
		std::size_t ReadBufferSize = 1024>
class Session_Static_Callback_Static_Share final :
		public Session_Base<Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>,
				UseSSL, InContainer, OutContainer, ReadBufferSize>
		, public Static_Shareable<Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>, false>
		, public std::enable_shared_from_this<Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>>
{
		using self_type = Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>;
		using base_type = Session_Base<Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>,
				UseSSL, InContainer, OutContainer, ReadBufferSize>;
		using shared_type = Static_Shareable<Session_Static_Callback_Static_Share<UseSSL, InContainer, OutContainer, ReadBufferSize>, false>;

	public:
		using on_open_t = std::function<void(std::shared_ptr<self_type>)>;
		using on_close_t = std::function<void(std::shared_ptr<self_type>, boost::system::error_code)>;
		using on_message_t = std::function<void(std::shared_ptr<self_type>, InContainer const&)>;
		using on_error_t = std::function<void(std::shared_ptr<self_type>, boost::system::error_code, char const*)>;

		using base_type::Session_Base;
		virtual ~Session_Static_Callback_Static_Share();

		static void write_all(InContainer const data);
		static void write_all(InContainer const data, bool text);

		static void set_on_open(on_open_t func = nullptr) noexcept;
		static void set_on_close(on_close_t func = nullptr) noexcept;
		static void set_on_message(on_message_t func = nullptr) noexcept;
		static void set_on_error(on_error_t func = nullptr) noexcept;
	protected:
		void on_open() noexcept override;
		void read_handler(InContainer data) noexcept override;
		void fail(boost::system::error_code ec, char const* what) noexcept override;

		static on_open_t on_open_f;
		static on_close_t on_close_f;
		static on_message_t on_message_f;
		static on_error_t on_error_f;
};

}//Websocket
}//My_Async

#include "impl/session_static_callback_static_share_impl.hpp"

#endif /* WEBSOCKET_SESSION_STATIC_SHARED_STATIC_CALLBACK_HPP__ */
