#ifndef MONITOR_APP_CLASS_HPP__
#define MONITOR_APP_CLASS_HPP__

#include <memory>
#include "boost/asio.hpp"

#include "proc_info.hpp"

namespace Apps {
namespace Monitor{

class Executor : public std::enable_shared_from_this<Executor>{
	public:
		Executor(boost::asio::io_context& ioc);
		~Executor();

		bool init();

		void sample(int samp);
		void cancel();

		void continuos(bool cont);
	private:
		void calculate();
		void do_wait(const boost::system::error_code& ec);

		boost::asio::io_context& ioc_;
		int sample_ = 1;
		boost::asio::steady_timer timer_;
		bool keep_reading_ = false;
		std::atomic<bool> is_running_ = false;

		System::Process_Info start_, end_;
		double uptime_ = 0;
};

}//Monitor
}//Apps

#endif /* MONITOR_APP_CLASS_HPP__ */
