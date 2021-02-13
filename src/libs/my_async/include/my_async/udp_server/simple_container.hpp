#ifndef UDP_SERVER_SIMPLE_CONTAINER_HPP__
#define UDP_SERVER_SIMPLE_CONTAINER_HPP__

#include <boost/asio.hpp>
#include <map>
#include <memory>
#include <functional>
#include <optional>

namespace My_Async{
namespace UDP{

template<typename Server>
class Simple_Container_Server{
	public:
		Simple_Container_Server(boost::asio::io_context& ioc) : ioc_(ioc){}

		template<typename ...Args>
		void open(boost::asio::ip::udp::endpoint const&,
				boost::system::error_code& ec,
				Args...);

		void close(boost::asio::ip::udp::endpoint const& server,
					boost::system::error_code&);

		std::vector<boost::asio::ip::udp::endpoint>
		endpoints();

		std::optional<std::shared_ptr<Server>>
		server(boost::asio::ip::udp::endpoint const&);

		template<typename OutContainer>
		void write(boost::asio::ip::udp::endpoint const& from,
					boost::asio::ip::udp::endpoint const& to,
					OutContainer const& data,
					boost::system::error_code& ec);

		template<typename OutContainer>
		void write(boost::asio::ip::udp::endpoint const& from,
					std::vector<boost::asio::ip::udp::endpoint> const& tos,
					OutContainer const& data,
					boost::system::error_code& ec);

		std::size_t size();
	protected:
		boost::asio::io_context& ioc_;
		std::map<boost::asio::ip::udp::endpoint, std::shared_ptr<Server>> servers_;
};

}//UDP
}//My_Async

#include "impl/simple_container_impl.hpp"

#endif /* UDP_SERVER_SIMPLE_CONTAINER_HPP__ */
