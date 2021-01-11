#ifndef READ_WAIT_ASYNC_HPP__
#define READ_WAIT_ASYNC_HPP__

#include <chrono>

#include "../../../../utility/include/types/traits.hpp"

namespace My_Async{
namespace Util{

template<typename InContainer,
		unsigned int ReadWait,
		typename Duration = std::chrono::milliseconds>
class Read_Wait{
	public:
    	static_assert(my_traits::is_chrono_duration<Duration>::value, "duration must be a std::chrono::duration");

		Read_Wait(boost::asio::io_context& ioc) : read_wait_timer_(ioc){};
		virtual ~Read_Wait(){}
	protected:
		virtual void read_time_handler(InContainer& data) noexcept = 0;

		virtual void read_handler(InContainer data) noexcept{
			static InContainer readed;

			readed.insert(readed.end(), data.begin(), data.end());

			read_wait_timer_.expires_after(Duration(ReadWait));
			read_wait_timer_.async_wait(
				[this](const boost::system::error_code& ec){
					if(!ec) {
						read_time_handler(readed);
						readed.clear();
					}
			});
		}

		boost::asio::steady_timer read_wait_timer_;
};

}//Util
}//My_Async


#endif /* READ_WAIT_ASYNC_HPP__ */
