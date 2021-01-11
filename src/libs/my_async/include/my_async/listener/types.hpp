#ifndef SESSION_LISTENER_TCP_TYPES_HPP__
#define SESSION_LISTENER_TCP_TYPES_HPP__

#include <boost/asio.hpp>

namespace My_Async{
namespace Listener{

using client_list = std::vector<boost::asio::ip::tcp::endpoint>;

enum class Secure_Type{
	plain = 0,
	ssl
};

}//Listener
}//My_Async


#endif /* SESSION_LISTENER_TCP_TYPES_HPP__ */
