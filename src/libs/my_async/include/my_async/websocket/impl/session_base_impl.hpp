#ifndef WEBSOCKET_SESSION_BASE_ASYNC_IMPL_HPP__
#define WEBSOCKET_SESSION_BASE_ASYNC_IMPL_HPP__

namespace My_Async{
namespace Websocket{

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
run() noexcept{
	boost::asio::dispatch(stream_.get_executor(),
		boost::beast::bind_front_handler(
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
on_run() noexcept{
	if constexpr(UseSSL == false){ //Plain
	 set_run_options();

	// Accept the websocket handshake
	stream_.async_accept(
		boost::beast::bind_front_handler(
			&self_type::on_accept,
			derived().shared_from_this()
		)
	);
	} else { //SSL
#if USE_SSL == 1
		// Set the timeout.
		boost::beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

		 // Perform the SSL handshake
		stream_.next_layer().async_handshake(
			boost::asio::ssl::stream_base::server,
			boost::beast::bind_front_handler(
				&self_type::on_handshake,
				derived().shared_from_this()));
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
	// Turn off the timeout on the tcp_stream, because
	// the websocket stream has its own timeout system.
	boost::beast::get_lowest_layer(stream_).expires_never();

	set_run_options();

	// Accept the websocket handshake
	stream_.async_accept(
		boost::beast::bind_front_handler(
			  &self_type::on_accept,
			derived().shared_from_this()));
}
#endif

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
close(const boost::beast::websocket::close_reason& reason) noexcept
{
	stream_.async_close(reason,
			boost::beast::bind_front_handler(
					&self_type::closed,
					derived().shared_from_this()));
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
closed(boost::beast::error_code const& ec)
{
	on_close(ec);
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
template<class Body, class Allocator>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
run(boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
{
	set_run_options();

	// Accept the websocket handshake
	stream_.async_accept(
		req,
		boost::beast::bind_front_handler(
			&self_type::on_accept,
			derived().shared_from_this()));
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
on_accept(boost::system::error_code ec){
	if(ec)
		return fail(ec, "accept");

	do_read();
	on_open();
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
write(std::shared_ptr<OutContainer const> const& data, bool text) noexcept
{
	this->text(text);
	write(data);
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
write(OutContainer const data) noexcept
{
	write(std::make_shared<OutContainer const>(std::move(data)));
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
write(OutContainer const data, bool text) noexcept
{
	this->text(text);
	write(std::make_shared<OutContainer const>(std::move(data)));
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
do_write() noexcept
{
	stream_.async_write(
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

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
set_run_options()
{
	// Set suggested timeout settings for the websocket
	stream_.set_option(
		boost::beast::websocket::stream_base::timeout::suggested(
			boost::beast::role_type::server));

	// Set a decorator to change the Server of the handshake
	stream_.set_option(boost::beast::websocket::stream_base::decorator(
		[](boost::beast::websocket::response_type& res)
		{
			res.set(boost::beast::http::field::server,
				std::string(BOOST_BEAST_VERSION_STRING) +
					" serial_websocket");
		}));
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
binary(bool b) noexcept
{
	stream_.binary(b);
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
void
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
text(bool b) noexcept
{
	stream_.text(b);
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
bool
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
binary() noexcept
{
	return stream_.binary();
}

template<typename Derived,
		bool UseSSL,
		typename InContainer,
		typename OutContainer,
		std::size_t ReadBufferSize>
bool
Session_Base<Derived, UseSSL, InContainer, OutContainer, ReadBufferSize>::
text() noexcept
{
	return stream_.text();
}

}//Websocket
}//My_Async

#endif /* WEBSOCKET_SESSION_BASE_ASYNC_IMPL_HPP__ */
