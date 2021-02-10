#ifndef UDP_CLIENT_SIMPLE_CONTAINER_HPP__
#define UDP_CLIENT_SIMPLE_CONTAINER_HPP__

#include <boost/asio.hpp>
#include <map>
#include <memory>
#include <functional>
#include <optional>

namespace My_Async{
namespace UDP{

template<typename Client>
class Simple_Container{
	public:
		Simple_Container(boost::asio::io_context& ioc) : ioc_(ioc){}

		template<typename ...Args>
		void open(boost::asio::ip::udp::endpoint const&,
				Args...);

		void close(boost::asio::ip::udp::endpoint const& client,
					boost::system::error_code&);

		std::vector<std::pair<boost::asio::ip::udp::endpoint, boost::asio::ip::udp::endpoint>>
		endpoints();

		std::optional<std::shared_ptr<Client>>
		client(boost::asio::ip::udp::endpoint const&);

		template<typename OutContainer>
		void write(boost::asio::ip::udp::endpoint const& ep,
					OutContainer const data,
					boost::system::error_code& ec);

		std::size_t size();
	protected:
		boost::asio::io_context& ioc_;
		std::map<boost::asio::ip::udp::endpoint, std::shared_ptr<Client>> clients_;
};

}//UDP
}//My_Async

#include "impl/simple_container_impl.hpp"

#endif /* UDP_CLIENT_SIMPLE_CONTAINER_HPP__ */
