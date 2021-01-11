#ifndef HTTP_SESSION_HPP__
#define HTTP_SESSION_HPP__

#include <boost/asio.hpp>
#include <boost/beast.hpp>

#if USE_SSL == 1
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#endif

#include <memory>

#include <iostream>

namespace My_Async{
namespace Listener{

template<typename SessionPlain, class Body, class Allocator>
static void
make_websocket_session(
    boost::beast::tcp_stream stream,
	boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
{
    std::make_shared<SessionPlain>(
        std::move(stream))->run(std::move(req));
}

template<typename SessionSSL, class Body, class Allocator>
static void
make_websocket_session(
	boost::beast::ssl_stream<boost::beast::tcp_stream> stream,
	boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>> req)
{
    std::make_shared<SessionSSL>(
        std::move(stream))->run(std::move(req));
}

//------------------------------------------------------------------------------

// Handles an HTTP server connection.
// This uses the Curiously Recurring Template Pattern so that
// the same code works with both SSL streams and regular sockets.
template<class Derived>
class http_session
{
    // Access the derived class, this is part of
    // the Curiously Recurring Template Pattern idiom.
    Derived&
    derived(){
        return static_cast<Derived&>(*this);
    }

    // The parser is stored in an optional container so we can
    // construct it from scratch it at the beginning of each new message.
	protected:
		boost::optional<
			boost::beast::http::request_parser<
				boost::beast::http::string_body>> parser_;
		boost::beast::flat_buffer buffer_;
		void fail(boost::system::error_code ec, const char* what){
			std::cerr << what << ": " << ec.message() << "\n";
		}

public:
    // Construct the session
    http_session(boost::beast::flat_buffer buffer)
        : buffer_(std::move(buffer)){}

    void
    do_read(){
        // Construct a new parser for each message
        parser_.emplace();

        // Apply a reasonable limit to the allowed size
        // of the body in bytes to prevent abuse.
        parser_->body_limit(10000);

        // Set the timeout.
        boost::beast::get_lowest_layer(
            derived().stream()).expires_after(std::chrono::seconds(30));

        // Read a request using the parser-oriented interface
        boost::beast::http::async_read(
            derived().stream(),
            buffer_,
            *parser_,
            boost::beast::bind_front_handler(
                &http_session::on_read,
                derived().shared_from_this()));
    }

    void
    on_read(boost::system::error_code ec, std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        // This means they closed the connection
        if(ec == boost::beast::http::error::end_of_stream){
        	derived().do_eof();
            return;
        }
        if(ec) return fail(ec, "read");

        // See if it is a WebSocket Upgrade
        if(boost::beast::websocket::is_upgrade(parser_->get())){
            // Disable the timeout.
            // The websocket::stream uses its own timeout settings.
            boost::beast::get_lowest_layer(derived().stream()).expires_never();

            // Create a websocket session, transferring ownership
            // of both the socket and the HTTP request.
            derived().make_websocket();
            return;
        }
    }
};

//------------------------------------------------------------------------------

// Handles a plain HTTP connection
template<typename SessionPlain>
class plain_http_session
    : public http_session<plain_http_session<SessionPlain>>
    , public std::enable_shared_from_this<plain_http_session<SessionPlain>>
{
    boost::beast::tcp_stream stream_;

public:
    // Create the session
    plain_http_session(
        boost::beast::tcp_stream&& stream,
        boost::beast::flat_buffer&& buffer)
        : http_session<plain_http_session<SessionPlain>>(
            std::move(buffer))
        , stream_(std::move(stream)){}

    // Start the session
    void
    run(){
        this->do_read();
    }

    // Called by the base class
    boost::beast::tcp_stream&
    stream(){
        return stream_;
    }

    void make_websocket(){
		make_websocket_session<SessionPlain>(std::move(stream_),
				http_session<plain_http_session<SessionPlain>>::parser_->release());
	}

    // Called by the base class
    void
    do_eof(){
        // Send a TCP shutdown
        boost::system::error_code ec;
        stream_.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_send, ec);

        // At this point the connection is closed gracefully
    }
};

//------------------------------------------------------------------------------

// Handles an SSL HTTP connection
template<typename SessionSSL>
class ssl_http_session
    : public http_session<ssl_http_session<SessionSSL>>
    , public std::enable_shared_from_this<ssl_http_session<SessionSSL>>
{
    boost::beast::ssl_stream<boost::beast::tcp_stream> stream_;

public:
    // Create the http_session
    ssl_http_session(
        boost::beast::tcp_stream&& stream,
        boost::asio::ssl::context& ctx,
        boost::beast::flat_buffer&& buffer)
        : http_session<ssl_http_session>(
            std::move(buffer))
        , stream_(std::move(stream), ctx){}

    // Start the session
    void
    run(){
        // Set the timeout.
        boost::beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

        // Perform the SSL handshake
        // Note, this is the buffered version of the handshake.
        stream_.async_handshake(
            boost::asio::ssl::stream_base::server,
			http_session<ssl_http_session<SessionSSL>>::buffer_.data(),
            boost::beast::bind_front_handler(
                &ssl_http_session::on_handshake,
                this->shared_from_this()));
    }

    // Called by the base class
    boost::beast::ssl_stream<boost::beast::tcp_stream>&
    stream(){
        return stream_;
    }

    void make_websocket(){
		make_websocket_session<SessionSSL>(std::move(stream_),
				http_session<ssl_http_session<SessionSSL>>::parser_->release());
	}


    // Called by the base class
    void
    do_eof(){
        // Set the timeout.
        boost::beast::get_lowest_layer(stream_).expires_after(std::chrono::seconds(30));

        // Perform the SSL shutdown
        stream_.async_shutdown(
            boost::beast::bind_front_handler(
                &ssl_http_session::on_shutdown,
                this->shared_from_this()));
    }

private:
    void fail(boost::system::error_code ec, const char* what){
    	std::cerr << what << ": " << ec.message() << "\n";
    }

    void
    on_handshake(
        boost::beast::error_code ec,
        std::size_t bytes_used)
    {
        if(ec) return fail(ec, "handshake");

        // Consume the portion of the buffer used by the handshake
        http_session<ssl_http_session<SessionSSL>>::
			buffer_.consume(bytes_used);

        http_session<ssl_http_session<SessionSSL>>::
			do_read();
    }

    void
    on_shutdown(boost::beast::error_code ec)
    {
        if(ec) return fail(ec, "shutdown");

        // At this point the connection is closed gracefully
    }
};

}//Listener
}//My_Async

#endif /* HTTP_SESSION_HPP__ */
