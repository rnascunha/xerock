#ifndef LOAD_CERTIFICATE_HPP__
#define LOAD_CERTIFICATE_HPP__

#include <boost/asio/ssl/context.hpp>

void
load_certificate(boost::asio::ssl::context& ctx,
		const std::string cert_file_name,
		const std::string key_file_name,
		boost::system::error_code& ec) noexcept;

void
load_certificate(boost::asio::ssl::context& ctx,
		const std::string cert_file_name,
		const std::string key_file_name);

void
load_certificate(boost::asio::ssl::context& ctx);

#endif /* LOAD_CERTIFICATE_HPP__ */
