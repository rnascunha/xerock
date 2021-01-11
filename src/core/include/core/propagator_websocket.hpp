#ifndef MAIN_APP_PROPAGATOR_WEBSOCKET_HPP__
#define MAIN_APP_PROPAGATOR_WEBSOCKET_HPP__

#include "my_async.hpp"
#include "byte_array.h"
#include "util/static_shareable.hpp"

#include "core/message.hpp"

namespace Core{

template<bool UseSSL>
class Propagator_Websocket final :
		public My_Async::Websocket::Session_Base<Propagator_Websocket<UseSSL>, UseSSL, Byte_Array>
		, public Static_Shareable<Propagator_Websocket<UseSSL>, false>
		, public std::enable_shared_from_this<Propagator_Websocket<UseSSL>>
{
		using self_type = Propagator_Websocket<UseSSL>;
		using base_type = My_Async::Websocket::Session_Base<
				Propagator_Websocket<UseSSL>, UseSSL, Byte_Array>;
		using shared_type = Static_Shareable<Propagator_Websocket<UseSSL>, false>;
	public:
		using base_type::Session_Base;
		~Propagator_Websocket();

		template<typename ErrorType>
		void fail_message(
				Message::App,
				ErrorType,
				const char* = "") noexcept;

		static void write_all(Byte_Array const data);
		static void write_all(Byte_Array const data, bool text);
	protected:
		void on_open() noexcept override;
		void read_handler(Byte_Array data) noexcept override;
		void fail(boost::system::error_code ec, char const* what) noexcept override;
};

}//Core

#include "json/propagator_websocket_impl.hpp"

#endif /* MAIN_APP_PROPAGATOR_WEBSOCKET_HPP__ */
