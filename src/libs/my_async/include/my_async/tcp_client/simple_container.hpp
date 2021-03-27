#ifndef TCP_CLIENT_SIMPLE_CONTAINER_HPP__
#define TCP_CLIENT_SIMPLE_CONTAINER_HPP__

#include <boost/asio.hpp>
#include <map>
#include <memory>
#include <functional>
#include <optional>

namespace My_Async{
namespace TCP{

template<typename Client>
class Simple_Container{
	public:
		Simple_Container(boost::asio::io_context& ioc) : ioc_(ioc){}

#if USE_SSL == 1
		Simple_Container(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx) :
			ioc_(ioc), ctx_(ctx){}
#endif
		template<typename Callback, typename ...Args>
		void open(boost::asio::ip::tcp::endpoint const&,
				Callback cb, Args...);

		template<typename ...Args>
		void open(boost::asio::ip::tcp::endpoint const&,
				Args...);

		void close(boost::asio::ip::tcp::endpoint const& client,
					boost::system::error_code&);

		std::vector<std::pair<boost::asio::ip::tcp::endpoint, boost::asio::ip::tcp::endpoint>>
		endpoints();

		std::optional<std::shared_ptr<Client>>
		client(boost::asio::ip::tcp::endpoint const&);

		template<typename OutContainer>
		void write(boost::asio::ip::tcp::endpoint const& ep,
					OutContainer const data,
					boost::system::error_code& ec);

		std::size_t size();
	protected:
		boost::asio::io_context& ioc_;

#if USE_SSL == 1
		//Check if the session is SSL. If not, exclude ctx_ attribute
		using ssl_context = my_traits::exclude_attr_if<
							Client::use_ssl, boost::asio::ssl::context&>;
		ssl_context ctx_;
#else
		using ssl_context = struct{};
#endif
		std::map<boost::asio::ip::tcp::endpoint, std::shared_ptr<Client>> clients_;
};

}//TCP
}//My_Async

#include "impl/simple_container_impl.hpp"

#endif /* TCP_CLIENT_SIMPLE_CONTAINER_HPP__ */
