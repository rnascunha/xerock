#ifndef MY_ASYNC_SERIAL_CONTAINER_HPP__
#define MY_ASYNC_SERIAL_CONTAINER_HPP__

#include "boost/asio.hpp"

#include <memory>
#include <map>
#include <string>

#include "serial_util.hpp"

namespace My_Async{
namespace Serial{

template<typename Serial_Type>
class Container
{
	public:
		Container(boost::asio::io_context& ioc) : ioc_(ioc){}
		virtual ~Container(){}

		bool is_open(std::string const&) const noexcept;

		template<typename Make_Func = std::shared_ptr<Serial_Type>(boost::asio::io_context&),
				typename ...Args>
		void open(std::string const& port_name,
				unsigned int baud,
				boost::system::error_code& ec
				, Make_Func = make_serial<Serial_Type>
				, Args...) noexcept;

		void close(std::string const& port_name,
				boost::system::error_code& ec) noexcept;

		template<typename OutContainer>
		void write(std::string const& port_name,
				OutContainer const data,
				boost::system::error_code& ec) noexcept;

		bool check_ports() noexcept;

		void config(std::string const&,
				unsigned int,
				boost::asio::serial_port_base::character_size const&,
				boost::asio::serial_port_base::parity const&,
				boost::asio::serial_port_base::stop_bits const&,
				boost::asio::serial_port_base::flow_control const&,
				boost::system::error_code&) noexcept;

		void get_config(std::string const&,
				unsigned int&,
				boost::asio::serial_port_base::character_size&,
				boost::asio::serial_port_base::parity&,
				boost::asio::serial_port_base::stop_bits&,
				boost::asio::serial_port_base::flow_control&,
				boost::system::error_code&) const noexcept;
	protected:
		std::map<std::string, std::shared_ptr<Serial_Type>> serials;
		boost::asio::io_context& ioc_;
};

}//Serial
}//My_Async

#include "impl/container_impl.hpp"

#endif /* MY_ASYNC_SERIAL_CONTAINER_HPP__ */
