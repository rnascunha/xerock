#ifndef DETECT_PROTOCOL_SESSION_HPP__
#define DETECT_PROTOCOL_SESSION_HPP__

#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast.hpp>

#include "http_session.hpp"

namespace My_Async{
namespace Listener{

template<typename SessionPlain,
		typename SessionSSL>
class Detect_Session
		: public std::enable_shared_from_this<Detect_Session<SessionPlain, SessionSSL>>
{
    boost::beast::tcp_stream stream_;
    boost::asio::ssl::context& ctx_;
    boost::beast::flat_buffer buffer_;
public:
    explicit
    Detect_Session(
    		boost::asio::ip::tcp::socket&& socket,
			boost::asio::ssl::context& ctx)
        : stream_(std::move(socket))
        , ctx_(ctx)
    {
    }

    // Launch the detector
    void
    run()
    {
        // Set the timeout.
        stream_.expires_after(std::chrono::seconds(30));

        boost::beast::async_detect_ssl(
            stream_, buffer_,
            boost::beast::bind_front_handler(
                &Detect_Session<SessionPlain, SessionSSL>::on_detect,
                this->shared_from_this()));
    }

    void
    on_detect(boost::system::error_code ec, bool result)
    {
        if(ec) return fail(ec, "detect");

        if(result){
            // Launch SSL session
            std::make_shared<ssl_http_session<SessionSSL>>(
                std::move(stream_),
                ctx_,
				std::move(buffer_))->run();
            return;
        }

        // Launch plain session
        std::make_shared<plain_http_session<SessionPlain>>(
            std::move(stream_),
        	std::move(buffer_))->run();
    }

    void
    fail(boost::system::error_code ec, char const* what){
        std::cerr << what << ": " << ec.message() << "\n";
    }
};

}//Listener
}//My_Async

#endif /* DETECT_PROTOCOL_SESSION_HPP__ */
