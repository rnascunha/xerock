#include "executor.hpp"

#include "make.hpp"
#include "core/helpers.hpp"
#include "core/error.hpp"

namespace Apps {
namespace Monitor{

Executor::Executor(boost::asio::io_context& ioc) :
	ioc_(ioc),
	timer_(ioc){}

Executor::~Executor()
{
	cancel();
}

void Executor::cancel()
{
	keep_reading_ = false;
	boost::system::error_code ec;
	timer_.cancel(ec);
}

void Executor::continuos(bool cont)
{
	keep_reading_ = cont;
}

bool Executor::init()
{
	bool f = false;
	if(is_running_.compare_exchange_strong(f, true)){
		calculate();
	}

	return is_running_;
}

void Executor::sample(int samp)
{
	sample_ = samp;
}

void Executor::calculate()
{
	start_.clear(); end_.clear();

	if(!process_info(start_))
	{
		Core::fail_message(Message::App::monitor, make_error_code(Message_Errc::SERVER_INTERNAL_ERROR));
		is_running_ = false;
		return;
	}

	if(!System::process_uptime(start_, uptime_))
	{
		Core::fail_message(Message::App::monitor, make_error_code(Message_Errc::SERVER_INTERNAL_ERROR));
		is_running_ = false;
		return;
	}

	timer_.expires_after(std::chrono::seconds(sample_));
	timer_.async_wait(std::bind(&Executor::do_wait,
					shared_from_this(), std::placeholders::_1));
}

void Executor::do_wait(const boost::system::error_code& ec)
{
	if(ec)
	{
		is_running_ = false;
		return;
	}

	if(!process_info(end_))
	{
		Core::fail_message(Message::App::monitor, make_error_code(Message_Errc::SERVER_INTERNAL_ERROR));
		return;
	}

	auto cpu_time = System::cpu_usage(start_, end_, true);
	double per_mem = System::memory_usage_percent();

	unsigned int num_threads = start_.tcusage.size();
	System::Memory_Peak_Usage peak_memory;
	System::memory_peak_usage(peak_memory);

	auto cpu_time_overall = System::cpu_usage(end_, true);
	auto cpu_time_overall_per = System::cpu_usage_percent(end_, true);

	std::string const msg = make_monitor_message(cpu_time, per_mem,
												cpu_time_overall,
												cpu_time_overall_per,
												uptime_, sample_,
												num_threads, peak_memory,
												start_, end_);

	if(!Core::Propagator::get_num_shares()) {
		is_running_ = false;
		return;
	}

	Core::Propagator::write_all(Byte_Array(msg), true);

	if(keep_reading_)
		calculate();
	else
		is_running_ = false;
}

}//Monitor
}//Apps
