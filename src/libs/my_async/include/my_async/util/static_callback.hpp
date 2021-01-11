#ifndef MY_ASYNC_UTIL_STATIC_CALLBACK_HPP__
#define MY_ASYNC_UTIL_STATIC_CALLBACK_HPP__

#include <functional>
#include <memory>

#include <boost/system/error_code.hpp>

namespace My_Async{
namespace Util{

template<class Comm, class InContainer>
class Static_Callback{
	public:
		using on_open_t = std::function<void(std::shared_ptr<Comm>)>;
		using on_close_t = std::function<void(std::shared_ptr<Comm>, boost::system::error_code)>;
		using on_message_t = std::function<void(std::shared_ptr<Comm>, InContainer const&)>;
		using on_error_t = std::function<void(std::shared_ptr<Comm>, boost::system::error_code, char const*)>;

		static void set_on_open(on_open_t func = nullptr) noexcept{
			on_open_f = func;
		}

		static void set_on_close(on_close_t func = nullptr) noexcept{
			on_close_f = func;
		}

		static void set_on_message(on_message_t func = nullptr) noexcept{
			on_message_f = func;
		}

		static void set_on_error(on_error_t func = nullptr) noexcept{
			on_error_f = func;
		}
	protected:
		static on_open_t on_open_f;
		static on_close_t on_close_f;
		static on_message_t on_message_f;
		static on_error_t on_error_f;
};

template<class Comm,
		typename InContainer>
typename Static_Callback<Comm, InContainer>::on_open_t
		Static_Callback<Comm, InContainer>::on_open_f;

template<class Comm,
		typename InContainer>
typename Static_Callback<Comm, InContainer>::on_close_t
		Static_Callback<Comm, InContainer>::on_close_f;

template<class Comm,
		typename InContainer>
typename Static_Callback<Comm, InContainer>::on_message_t
		Static_Callback<Comm, InContainer>::on_message_f;

template<class Comm,
		typename InContainer>
typename Static_Callback<Comm, InContainer>::on_error_t
		Static_Callback<Comm, InContainer>::on_error_f;

}//Util
}//My_Async

#endif /* MY_ASYNC_UTIL_STATIC_CALLBACK_HPP__ */
