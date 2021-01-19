#ifndef TCP_CLIENT_CONTAINER_HPP__
#define TCP_CLIENT_CONTAINER_HPP__

#include <cstddef>
#include <map>
#include <memory>
#include <tuple>
#include <vector>

#include <boost/asio.hpp>

#if USE_SSL == 1
#include <boost/asio/ssl.hpp>
#endif

//#include <type_traits>

#include "impl/client_concept_impl.hpp"

/**
 * Este conceito só funciona para o metodo write com Byte_Array como Argumento.
 * O ideal seria generalizar para qualquer tipo
 */

namespace My_Async{
namespace TCP{

template<bool SupportSSL = true, bool SupportPlain = true>
class Container{
	static_assert(SupportSSL == true || SupportPlain == true,
			"At least one type of protocol must be support by the Container");
	public:
		static constexpr const bool support_plain = SupportPlain;
		static constexpr const bool support_ssl = SupportSSL;

		Container(boost::asio::io_context& ioc)
			: ioc_(ioc){}

#if USE_SSL == 1
		Container(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx)
			: ioc_(ioc), ctx_(ctx){}
#endif
		template<typename Client, typename Callback, typename ...Args>
		void open(boost::asio::ip::tcp::endpoint const&,
				boost::system::error_code&,
				Callback cb, Args...);

		template<typename Client, typename ...Args>
		void open(boost::asio::ip::tcp::endpoint const&,
				boost::system::error_code&,
				Args...);

		void close(boost::asio::ip::tcp::endpoint const& client,
					boost::system::error_code&);

		std::vector<std::tuple<bool, boost::asio::ip::tcp::endpoint, boost::asio::ip::tcp::endpoint>>
		endpoints();

		template<typename OutContainer>
		void write(boost::asio::ip::tcp::endpoint const& ep,
					OutContainer const data,
					boost::system::error_code& ec);

		std::size_t size();
	private:
		boost::asio::io_context& ioc_;

#if USE_SSL == 1
		using ssl_context = my_traits::exclude_attr_if<
				SupportSSL, boost::asio::ssl::context&>;
		ssl_context ctx_;
#else
		using ssl_context = struct{};
#endif
		std::map<boost::asio::ip::tcp::endpoint, std::shared_ptr<Client_Concept>> clients_;
};

}//TCP
}//My_Async

#include "impl/container_impl.hpp"

#endif /* TCP_CLIENT_CONTAINER_HPP__ */
