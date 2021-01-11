#include "../container.hpp"
#include "boost/system/error_code.hpp"

#include <memory>

namespace My_Async{
namespace Serial{

template<typename Serial_Type>
bool
Container<Serial_Type>::
is_open(std::string const& port_name) const noexcept
{
	auto serial = serials.find(port_name);
	if(serial == serials.end()){
		return false;
	}
	return serial->second->is_open();
}

template<typename Serial_Type>
template<typename Make_Func, typename ...Args>
void
Container<Serial_Type>::
open(std::string const& port_name,
					unsigned int baud,
					boost::system::error_code& ec
					, Make_Func make
					, Args ...args
					) noexcept
{
	auto serial_ = serials.find(port_name);
	if(serial_ != serials.end())
	{
		ec = boost::system::errc::make_error_code(boost::system::errc::already_connected);
		return;
	}

	auto serial = make(ioc_, args...);
	serial->open(port_name.c_str(), baud, ec);
	if(ec) return;

	serials[port_name] = serial;
}

template<typename Serial_Type>
void
Container<Serial_Type>::
close(std::string const& port_name,
		boost::system::error_code& ec) noexcept
{
	auto serial = serials.find(port_name);
	if(serial == serials.end())
	{
		ec = boost::system::errc::make_error_code(boost::system::errc::not_connected);
		return;
	}
	if(serial->second->is_open())
		serial->second->close(ec);
	serials.erase(serial);
}

template<typename Serial_Type>
template<typename OutContainer>
void
Container<Serial_Type>::
write(std::string const& port_name, OutContainer const data,
		boost::system::error_code& ec) noexcept
{
	auto serial = serials.find(port_name);
	if(serial == serials.end())
	{
		ec = boost::system::errc::make_error_code(boost::system::errc::not_connected);
		return;
	}

	serial->second->write(std::move(data));
}

template<typename Serial_Type>
void
Container<Serial_Type>::
config(std::string const& port_name,
		unsigned int baud,
		boost::asio::serial_port_base::character_size const& ch,
		boost::asio::serial_port_base::parity const& parity,
		boost::asio::serial_port_base::stop_bits const& sb,
		boost::asio::serial_port_base::flow_control const& fw,
		boost::system::error_code& ec) noexcept
{
	auto serial = serials.find(port_name);
	if(serial == serials.end())
	{
		ec = boost::system::errc::make_error_code(boost::system::errc::not_connected);
		return;
	}

	serial->second->set_option(boost::asio::serial_port_base::baud_rate{baud}, ec);
	if(ec) return;
	serial->second->set_option(ch, ec);
	if(ec) return;
	serial->second->set_option(parity, ec);
	if(ec) return;
	serial->second->set_option(sb, ec);
	if(ec) return;
	serial->second->set_option(fw, ec);
	if(ec) return;
}

template<typename Serial_Type>
void
Container<Serial_Type>::
get_config(std::string const& port_name,
		unsigned int& baud,
		boost::asio::serial_port_base::character_size& size,
		boost::asio::serial_port_base::parity& parity,
		boost::asio::serial_port_base::stop_bits& stopbit,
		boost::asio::serial_port_base::flow_control& flowcontrol,
		boost::system::error_code& ec) const noexcept
{
	auto serial = serials.find(port_name);
	if(serial == serials.end())
	{
		ec = boost::system::errc::make_error_code(boost::system::errc::not_connected);
		return;
	}

	boost::asio::serial_port_base::baud_rate br;
	serial->second->get_option(br, ec);
	if(ec) return;
	baud = br.value();

	serial->second->get_option(size, ec);
	if(ec) return;
	serial->second->get_option(parity, ec);
	if(ec) return;
	serial->second->get_option(stopbit, ec);
	if(ec) return;
	serial->second->get_option(flowcontrol, ec);
	if(ec) return;
}

template<typename Serial_Type>
bool
Container<Serial_Type>::
check_ports() noexcept
{
	bool flag = false;
	for(auto& s : serials){
		if(!s.second->is_open())
		{
			flag = true;
			boost::system::error_code ec;
			s.second->close(ec);
		}
	}
	return flag;
}

}//Serial
}//My_Async
