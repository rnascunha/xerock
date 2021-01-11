#ifndef SESSION_LISTENER_TCP_CONTAINER_HPP__
#define SESSION_LISTENER_TCP_CONTAINER_HPP__

#include <cstddef>
#include <map>
#include <memory>
#include <vector>

#include "byte_array.h"

#include "types.hpp"

#include <boost/asio.hpp>

#if USE_SSL == 1
#include <boost/asio/ssl.hpp>
#endif

#include <type_traits>

#include "impl/listener_concept_impl.hpp"
#include "listener_helper.hpp"

/**
 * Este conceito só funciona para o metodo write com Byte_Array como Argumento.
 * O ideal seria generalizar para qualquer tipo
 */

namespace My_Async{
namespace Listener{

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
			: ioc_(ioc), ctx(ctx){}
#endif

		bool is_open(boost::asio::ip::tcp::endpoint const& ep);

		/**
		 * How to set default constructor (Make_Func) to both kind
		 * of Session (plain and ssl)?
		 */
		template<typename Session, typename Make_Func, typename ...Args>
		bool open(boost::asio::ip::tcp::endpoint const& ep,
							boost::system::error_code& ec,
							Make_Func,
							Args...);

		void close(boost::asio::ip::tcp::endpoint const& ep,
				boost::system::error_code& ec);

		void write(boost::asio::ip::tcp::endpoint const& ep, Byte_Array const data,
				boost::system::error_code& ec);

		std::vector<std::pair<bool, boost::asio::ip::tcp::endpoint>>
		endpoints();

		client_list
		clients(boost::asio::ip::tcp::endpoint const&, boost::system::error_code&);

		void
		close_clients(boost::asio::ip::tcp::endpoint const&, client_list, boost::system::error_code&);

		template<typename OutContainer>
		void write(boost::asio::ip::tcp::endpoint const& from,
							client_list const& to,
							OutContainer const data,
							boost::system::error_code& ec);

		std::size_t count();
	private:
		boost::asio::io_context& ioc_;

#if USE_SSL == 1
		using ssl_context = my_traits::exclude_attr_if<
				SupportSSL, boost::asio::ssl::context&>;
		ssl_context ctx;
#else
		using ssl_context = struct{};
#endif
		std::map<boost::asio::ip::tcp::endpoint, std::shared_ptr<Listener_Concept>> servers_;
};

}//Listener
}//My_Async

#include "impl/container_impl.hpp"

#endif /* SESSION_LISTENER_TCP_CONTAINER_HPP__ */
