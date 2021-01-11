#ifndef SERIAL_ASYNC_STATIC_CALLBACK_HPP__
#define SERIAL_ASYNC_STATIC_CALLBACK_HPP__

#include "serial_base.hpp"

#include <memory>
#include <functional>

namespace My_Async{
namespace Serial{

template<typename InContainer,
		typename OutContainer = InContainer,
		std::size_t ReadBufferSize = 1024>
class Serial_Static_Callback final :
		public Serial_Base<Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>,
				InContainer, OutContainer, ReadBufferSize>,
		public std::enable_shared_from_this<Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>>
{
		using base_type = Serial_Base<Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>,
						InContainer, OutContainer, ReadBufferSize>;
		using self_type = Serial_Static_Callback<InContainer, OutContainer, ReadBufferSize>;
	public:
		using on_open_t = std::function<void(std::shared_ptr<self_type>)>;
		using on_close_t = std::function<void(std::shared_ptr<self_type>,
												boost::system::error_code)>;
		using on_message_t = std::function<void(std::shared_ptr<self_type>,
												InContainer const&)>;
		using on_error_t = std::function<void(std::shared_ptr<self_type>,
										boost::system::error_code, char const*)>;

	    //Importing contructors
		using base_type::Serial_Base;

	    virtual ~Serial_Static_Callback(){}

	    static void set_on_open(on_open_t func = nullptr) noexcept;
	    static void set_on_close(on_close_t func = nullptr) noexcept;
	    static void set_on_message(on_message_t func = nullptr) noexcept;
	    static void set_on_error(on_error_t func = nullptr) noexcept;

	protected:
	    static on_open_t on_open_f;
	    static on_close_t on_close_f;
	    static on_message_t on_message_f;
		static on_error_t on_error_f;

	    virtual void read_handler(InContainer data) noexcept override;
	    virtual void fail(boost::system::error_code, const char*) noexcept override;

		virtual void on_open() noexcept override;
		virtual void on_close(boost::system::error_code) noexcept override;
};

}//Serial
}//My_Async

#include "impl/serial_static_callback_impl.hpp"

#endif /* SERIAL_ASYNC_STATIC_CALLBACK_HPP__ */
