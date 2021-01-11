#ifndef SESSION_LISTENER_TCP_SIMPLE_CONTAINER_HPP__
#define SESSION_LISTENER_TCP_SIMPLE_CONTAINER_HPP__

#include <cstddef>
#include <map>
#include <memory>
#include <vector>

#include "types.hpp"

#include "listener_base.hpp"
#include "types/traits.hpp"

#include <boost/asio.hpp>

namespace My_Async{
namespace Listener{

template<typename Listener>
class Simple_Container{
	public:
		Simple_Container(boost::asio::io_context& ioc) : ioc_(ioc){}

#if USE_SSL == 1
		Simple_Container(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx) :
			ioc_(ioc), ctx_(ctx){}
#endif
		bool is_open(boost::asio::ip::tcp::endpoint const& ep);

		bool open(boost::asio::ip::tcp::endpoint const& ep,
					boost::system::error_code& ec);

		void close(boost::asio::ip::tcp::endpoint const& ep,
				boost::system::error_code& ec);

		std::vector<std::pair<bool, boost::asio::ip::tcp::endpoint>>
		endpoints();

		client_list
		clients(boost::asio::ip::tcp::endpoint const&, boost::system::error_code&);

		void
		close_clients(boost::asio::ip::tcp::endpoint const&,
						client_list,
						boost::system::error_code& ec);

		template<typename OutContainer>
		void write(boost::asio::ip::tcp::endpoint const& from,
							client_list const& to,
							OutContainer const data,
							boost::system::error_code& ec);

		std::size_t count(){
			return servers_.size();
		}
	private:
		boost::asio::io_context& ioc_;

#if USE_SSL == 1
		//Check if the session is SSL. If not, exclude ctx_ attribute
		using ssl_context = my_traits::exclude_attr_if<
				Listener::use_ssl, boost::asio::ssl::context&>;
		ssl_context ctx_;
#else
		using ssl_context = struct{};
#endif
		std::map<boost::asio::ip::tcp::endpoint, std::shared_ptr<Listener>> servers_;
};

}//Listener
}//My_Async

#include "impl/simple_container_impl.hpp"

#endif /* SESSION_LISTENER_TCP_SIMPLE_CONTAINER_HPP__ */
