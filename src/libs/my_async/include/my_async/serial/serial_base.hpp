#ifndef SERIAL_BASE_ASYNC_HPP__
#define SERIAL_BASE_ASYNC_HPP__

#include "boost/asio.hpp"
#include <string>

namespace My_Async{
namespace Serial{

template<typename Derived,
		typename InContainer,
		typename OutContainer = InContainer,
		std::size_t ReadBufferSize = 1024>
class Serial_Base
{
private:
		Derived&
		derived(){
			return static_cast<Derived&>(*this);
		}

		using self_type = Serial_Base<Derived, InContainer, OutContainer, ReadBufferSize>;

	public:
		Serial_Base(boost::asio::io_context& ioc) :
					stream_(ioc){}

		virtual ~Serial_Base(){}

		bool is_open() const noexcept;

		virtual void open(const char* port_name, unsigned int baud,
							boost::system::error_code& ec) noexcept;
		virtual void open(const char* port_name, unsigned int baud);

		virtual void close(boost::system::error_code& ec) noexcept;
		virtual void close();

		virtual void cancel(boost::system::error_code& ec) noexcept;
		virtual void cancel();

		virtual void write(std::shared_ptr<OutContainer const> const& data) noexcept;
		virtual void write(OutContainer const data) noexcept;

		//Setters
		template<typename Option>
		void set_option(Option const&, boost::system::error_code&) noexcept;
		template<typename Option>
		void set_option(Option const&);

		//Getters
		template<typename Option>
		void get_option(Option&, boost::system::error_code&) noexcept;
		template<typename Option>
		void get_option(Option&);

		std::string const& get_port_name() const;
	protected:
		virtual void do_write() noexcept;
		virtual void on_writing(std::shared_ptr<OutContainer const> const& data) noexcept;
		virtual void on_write(boost::system::error_code ec,
				std::size_t bytes_transferred) noexcept;

		virtual void do_read() noexcept;
		virtual void on_read(boost::system::error_code ec,
					std::size_t bytes_transfered) noexcept;

		virtual void read_handler(InContainer data) noexcept = 0;
		virtual void fail(boost::system::error_code, const char*) noexcept = 0;

		virtual void on_open() noexcept{}
		virtual void on_close(boost::system::error_code) noexcept{}

		boost::asio::serial_port stream_;
		std::string port_name_;

		std::vector<std::shared_ptr<OutContainer const>> queue_;
		InContainer buffer_;
};

} //Serial
} //My_Async

#include "impl/serial_base_impl.hpp"

#endif /* SERIAL_BASE_ASYNC_HPP__ */
