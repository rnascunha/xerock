#ifndef UDP_CLIENT_BASE_SESSION_IMPL_HPP__
#define UDP_CLIENT_BASE_SESSION_IMPL_HPP__

#include "../client_base.hpp"

using boost::asio::ip::udp;

namespace My_Async{
namespace UDP{

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
Client_Base<Derived, InContainer, OutContainer, ReadBufferSize>::
Client_Base(boost::asio::io_context& ioc, const udp::endpoint& ep)
	: socket_(ioc, ep){}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
Client_Base<Derived, InContainer, OutContainer, ReadBufferSize>::
Client_Base(boost::asio::io_context& ioc)
	: socket_(ioc, udp::endpoint(udp::v4(), 0)){}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
Client_Base<Derived, InContainer, OutContainer, ReadBufferSize>::
Client_Base(boost::asio::io_context& ioc, unsigned short port)
	: socket_(ioc, udp::endpoint(udp::v4(), port)){}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Client_Base<Derived, InContainer, OutContainer, ReadBufferSize>::
open(udp::endpoint const& endpoint) noexcept
{
	endpoint_ = endpoint;

	on_open();
	do_read();
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Client_Base<Derived, InContainer, OutContainer, ReadBufferSize>::
write(std::shared_ptr<OutContainer const> data) noexcept
{
	boost::asio::post(
			socket_.get_executor(),
			std::bind(
				&self_type::writing,
				derived().shared_from_this(),
				data)
		);
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Client_Base<Derived, InContainer, OutContainer, ReadBufferSize>::
write(OutContainer const data) noexcept
{
	write(std::make_shared<OutContainer const>(std::move(data)));
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Client_Base<Derived, InContainer, OutContainer, ReadBufferSize>::
close(boost::system::error_code& ec) noexcept
{
	socket_.close(ec);
	on_close(ec);
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
udp::endpoint
Client_Base<Derived, InContainer, OutContainer, ReadBufferSize>::
endpoint() const
{
	return endpoint_;
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
udp::endpoint
Client_Base<Derived, InContainer, OutContainer, ReadBufferSize>::
local_endpoint() const
{
	return socket_.local_endpoint();
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Client_Base<Derived, InContainer, OutContainer, ReadBufferSize>::
fail(boost::system::error_code ec, char const* what) noexcept
{
	if(ec == boost::asio::error::operation_aborted ||
		ec == boost::asio::error::bad_descriptor)
		return;

	on_error(ec, what);
	close(ec);
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Client_Base<Derived, InContainer, OutContainer, ReadBufferSize>::
do_read() noexcept
{
	socket_.async_receive_from(
		boost::asio::dynamic_buffer(buffer_).prepare(ReadBufferSize),
		endpoint_,
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
Client_Base<Derived, InContainer, OutContainer, ReadBufferSize>::
on_read(boost::system::error_code ec, std::size_t bytes_transfered) noexcept
{
	if(ec)
		return fail(ec, "read");

	buffer_.resize(bytes_transfered);
	read_handler(buffer_);
	buffer_.erase(buffer_.begin(), buffer_.end());

	do_read();
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Client_Base<Derived, InContainer, OutContainer, ReadBufferSize>::
writing(std::shared_ptr<OutContainer const> const data) noexcept
{
	queue_.push_back(data);

	if(queue_.size() > 1) return;

	do_write();
}

template<typename Derived,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Client_Base<Derived, InContainer, OutContainer, ReadBufferSize>::
do_write() noexcept{
	socket_.async_send_to(
			boost::asio::buffer(*queue_.front()),
			endpoint_,
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
Client_Base<Derived, InContainer, OutContainer, ReadBufferSize>::
on_write(boost::system::error_code ec, std::size_t) noexcept
{
	if(ec)
		return fail(ec, "write");

	queue_.erase(queue_.begin());

	if(! queue_.empty())
		do_write();
}

}//UDP
}//My_Async

#endif /* UDP_CLIENT_BASE_SESSION_IMPL_HPP__ */
