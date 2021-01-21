#ifndef MY_ASYNC_TCP_SERVER_SESSION_IMPL_HPP__
#define MY_ASYNC_TCP_SERVER_SESSION_IMPL_HPP__

#include "../../util/tcp_options.hpp"

namespace My_Async{
namespace TCP{

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
run() noexcept{
	boost::asio::dispatch(stream_.get_executor(),
		std::bind(
			&self_type::on_run,
			derived().shared_from_this()
		)
	);
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
close()
{
	boost::asio::post(stream_.get_executor(),
					std::bind(&self_type::closing,
							derived().shared_from_this()));
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_run() noexcept{
	if constexpr(UseSSL == false){ //Plain
		// Accept the tcp handshake
		do_accept();
	} else { //SSL
#if USE_SSL == 1
		stream_.async_handshake(
			boost::asio::ssl::stream_base::server,
			std::bind(
				&self_type::on_handshake,
				derived().shared_from_this(),
				std::placeholders::_1));
#endif
	}
}

#if USE_SSL == 1
template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_handshake(boost::system::error_code ec){
	if(ec){
		fail(ec, "handshake");
		return;
	}

	do_accept();
}
#endif

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
bool
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
keep_alive() const noexcept
{
	boost::asio::socket_base::keep_alive option;
	if constexpr (use_ssl)
		stream_.lowest_layer().get_option(option);
	else
		stream_.get_option(option);

	return option.value();
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
keep_alive(bool enable) noexcept
{
	boost::asio::socket_base::keep_alive option(enable);
	if constexpr (use_ssl)
		stream_.lowest_layer().set_option(option);
	else
		stream_.set_option(option);
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
keep_alive(int32_t idle, int32_t count, int32_t interval, bool set /* = true */) noexcept
{
	keep_alive(set);
	if constexpr (use_ssl)
		keepalive(stream_.lowest_layer().native_handle(), idle, count, interval);
	else
		keepalive(stream_.native_handle(), idle, count, interval);
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
boost::asio::ip::tcp::endpoint
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
endpoint() const
{
	if constexpr(use_ssl)
		return stream_.lowest_layer().remote_endpoint();
	else
		return stream_.remote_endpoint();
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
boost::asio::ip::tcp::endpoint
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
local_endpoint() const
{
	if constexpr(use_ssl)
		return stream_.lowest_layer().local_endpoint();
	else
		return stream_.local_endpoint();
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
closing() noexcept
{
	boost::system::error_code ec;

	if constexpr(use_ssl)
		stream_.lowest_layer().shutdown(boost::asio::socket_base::shutdown_both, ec);
	else stream_.shutdown(boost::asio::socket_base::shutdown_both, ec);
	if(ec) return;

	if constexpr(use_ssl)
		stream_.lowest_layer().cancel(ec);
	else stream_.cancel(ec);
	if(ec) return;

	if constexpr(use_ssl)
		stream_.lowest_layer().close(ec);
	else stream_.close(ec);
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
do_accept() noexcept
{
	on_open();
	do_read();
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
fail(boost::system::error_code ec, char const* what) noexcept
{
	if(ec == boost::asio::error::operation_aborted
		||	ec == boost::asio::error::eof){
		on_close(ec);
		return;
	}
	if constexpr(UseSSL){
#if USE_SSL == 1
		if(ec == boost::asio::ssl::error::stream_truncated){
			on_close(ec);
			return;
		}
#endif
	}

	on_error(ec, what);
	on_close(ec);
	close();
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
do_read() noexcept{
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
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
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
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
write(std::shared_ptr<OutContainer const> const& data) noexcept
{
    boost::asio::post(
		stream_.get_executor(),
        std::bind(
            &self_type::writing,
			derived().shared_from_this(),
            data)
    );
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
write(OutContainer const data) noexcept{
	write(std::make_shared<OutContainer const>(std::move(data)));
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
do_write() noexcept{
	boost::asio::async_write(stream_,
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
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
writing(std::shared_ptr<OutContainer const> const& data) noexcept
{
    queue_.push_back(data);

    if(queue_.size() > 1) return;

    do_write();
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_write(boost::system::error_code ec, std::size_t) noexcept
{
    if(ec) return fail(ec, "write");

    queue_.erase(queue_.begin());

    if(! queue_.empty()){
        do_write();
	}
}

}//TCP
}//My_Async

#endif /* MY_ASYNC_TCP_SERVER_SESSION_IMPL_HPP__ */
