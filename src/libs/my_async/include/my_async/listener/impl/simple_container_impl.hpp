#ifndef SESSION_LISTENER_TCP_SIMPLE_CONTAINER_IMPL_HPP__
#define SESSION_LISTENER_TCP_SIMPLE_CONTAINER_IMPL_HPP__

#include "../simple_container.hpp"
#include "../listener_helper.hpp"

namespace My_Async{
namespace Listener{

template<typename Listener>
bool
Simple_Container<Listener>::
is_open(boost::asio::ip::tcp::endpoint const& ep)
{
	return servers_.find(ep) != servers_.end();
}

template<typename Listener>
bool
Simple_Container<Listener>::
open(boost::asio::ip::tcp::endpoint const& ep,
					boost::system::error_code& ec)
{
	if(is_open(ep)){
		ec = boost::system::errc::make_error_code(boost::system::errc::address_in_use);
		return false;
	}

	if constexpr(Listener::use_ssl){
		//SSL
#if USE_SSL == 1
		auto server = make_listener<
				typename Listener::Session_Type, Listener::has_strand>(ioc_, ctx_, ep, ec);
		if(ec)
			return false;

		servers_[ep] = server;
#endif
	} else {
		//Plain
		auto server = make_listener<
				typename Listener::Session_Type, Listener::has_strand>(ioc_, ep, ec);

		if(ec)
			return false;

		servers_[ep] = server;
	}

	return true;
}

template<typename Listener>
void Simple_Container<Listener>::
close(boost::asio::ip::tcp::endpoint const& ep,
		boost::system::error_code& ec)
{
	auto server = servers_.find(ep);
	if(server == servers_.end()){
		ec = boost::system::errc::make_error_code(boost::system::errc::not_connected);
		return;
	}

	server->second->close(ec, true);
	if(ec) return;

	servers_.erase(ep);
}

template<typename Listener>
template<typename OutContainer>
void
Simple_Container<Listener>::
write(boost::asio::ip::tcp::endpoint const& from,
					client_list const& to,
					OutContainer const data,
					boost::system::error_code& ec)
{
	auto server = servers_.find(from);
	if(server == servers_.end()){
		ec = boost::system::errc::make_error_code(boost::system::errc::not_connected);
		return;
	}
	auto const share_data = std::make_shared<OutContainer const>(data);

	for(auto const& t : to)
		server->second->write(t, share_data);
}

template<typename Listener>
client_list
Simple_Container<Listener>::
clients(boost::asio::ip::tcp::endpoint const& ep,
		boost::system::error_code& ec)
{
	auto server = servers_.find(ep);
	if(server == servers_.end()){
		ec = boost::system::errc::make_error_code(boost::system::errc::not_connected);
		return client_list{};
	}

	return server->second->clients();
}

template<typename Listener>
void
Simple_Container<Listener>::
close_clients(boost::asio::ip::tcp::endpoint const& ep, client_list clients, boost::system::error_code& ec)
{
	auto server = servers_.find(ep);
	if(server == servers_.end())
	{
		ec = boost::system::errc::make_error_code(boost::system::errc::not_connected);
		return;
	}

	server->second->close_clients(clients, ec);
}

template<typename Listener>
std::vector<std::pair<bool, boost::asio::ip::tcp::endpoint>>
Simple_Container<Listener>::
endpoints(){
	std::vector<std::pair<bool, boost::asio::ip::tcp::endpoint>> v;

	for (auto [key, value] : servers_) {
	    v.push_back({value->use_ssl, key});
	}

	return v;
}

}//Listener
}//My_Async

#endif /* SESSION_LISTENER_TCP_SIMPLE_CONTAINER_IMPL_HPP__ */
