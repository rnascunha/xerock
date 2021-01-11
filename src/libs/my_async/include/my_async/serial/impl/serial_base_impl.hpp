#ifndef SERIAL_BASE_ASYNC_IMPL_HPP__
#define SERIAL_BASE_ASYNC_IMPL_HPP__

#include "../serial_exception.hpp"
#include <string>

namespace My_Async{
namespace Serial{

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
bool
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
is_open() const noexcept
{
	return stream_.is_open();
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
open(const char* port_name, unsigned int baud, boost::system::error_code& ec) noexcept
{
	if(is_open())
	{
		ec = boost::system::errc::make_error_code(boost::system::errc::already_connected);
		return fail(ec, "open");
	}

	stream_.open(port_name, ec);
	if(ec) return fail(ec, "open");

	this->port_name_ = port_name;

	stream_.set_option(
		boost::asio::serial_port_base::baud_rate(baud), ec);
	if(ec) return fail(ec, "baudrate");

	do_read();

	on_open();
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
open(const char* port_name, unsigned int baud)
{
	boost::system::error_code ec;
	open(port_name, baud, ec);

	if(ec) throw Serial_Exception(ec.value(), ec.message());
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
close(boost::system::error_code& ec) noexcept
{
	if(!stream_.is_open()) return;

	stream_.close(ec);
	if(ec) fail(ec, "close");

	on_close(ec);

	port_name_.clear();
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
close()
{
	boost::system::error_code ec;
	close(ec);
	if(ec) throw Serial_Exception(ec.value(), ec.message());
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
cancel(boost::system::error_code& ec) noexcept
{
	stream_.cancel(ec);
	if(ec) return fail(ec, "cancel");
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
cancel()
{
	boost::system::error_code ec;
	cancel(ec);
	if(ec) throw Serial_Exception(ec.value(), ec.message());
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
write(OutContainer const data) noexcept
{
	write(std::make_shared<OutContainer const>(std::move(data)));
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
write(std::shared_ptr<OutContainer const> const& data) noexcept
{
    boost::asio::post(
		stream_.get_executor(),
        std::bind(
            &self_type::on_writing,
			derived().shared_from_this(),
            data)
    );
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
template<typename Option>
void
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
set_option(Option const& op, boost::system::error_code& ec) noexcept
{
	stream_.set_option(op, ec);
	if(ec) return fail(ec, "set_option");
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
template<typename Option>
void
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
set_option(Option const& op)
{
	boost::system::error_code ec;

	stream_.set_option(op, ec);
	if(ec) throw Serial_Exception(ec.value(), ec.message());
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
template<typename Option>
void
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
get_option(Option& op, boost::system::error_code& ec) noexcept
{
	stream_.get_option(op, ec);
	if(ec) return fail(ec, "get_option");
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
template<typename Option>
void
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
get_option(Option& op)
{
	boost::system::error_code ec;

	stream_.get_option(op, ec);
	if(ec) throw Serial_Exception(ec.value(), ec.message());
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
std::string const&
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
get_port_name() const
{
	return port_name_;
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
do_write() noexcept
{
	stream_.async_write_some(
		boost::asio::buffer(*queue_.front()),
		std::bind(
			&self_type::on_write,
			derived().shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2
		)
	);
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
on_writing(std::shared_ptr<OutContainer const> const& data) noexcept
{
    queue_.push_back(data);

    if(queue_.size() > 1)
        return;

    do_write();
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
on_write(boost::system::error_code ec, std::size_t) noexcept
{
    if(ec)
        return fail(ec, "write");

    queue_.erase(queue_.begin());

    if(! queue_.empty()){
        do_write();
	}
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
do_read() noexcept
{
	stream_.async_read_some(
		boost::asio::dynamic_buffer(buffer_).prepare(ReadBufferSize),
		std::bind(
			&self_type::on_read,
			derived().shared_from_this(),
			std::placeholders::_1,	//error_code
			std::placeholders::_2	//bytes transfered
		)
	);
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Serial_Base<Derived,InContainer, OutContainer, ReadBufferSize>::
on_read(boost::system::error_code ec, std::size_t bytes_transfered) noexcept
{
	if(ec)
		return fail(ec, "read");

	buffer_.resize(bytes_transfered);
	read_handler(buffer_);
	buffer_.clear();

	do_read();
}

} //Serial
} //My_Async

#endif /* SERIAL_BASE_ASYNC_IMPL_HPP__ */
