#ifndef SESSION_LISTENER_TCP_CONTAINER_IMPL_HPP__
#define SESSION_LISTENER_TCP_CONTAINER_IMPL_HPP__

#include "../container.hpp"

namespace My_Async{
namespace Listener{

template<bool SupportSSL, bool SupportPlain>
template<typename Session, typename Make_Func, typename ...Args>
bool
Container<SupportSSL, SupportPlain>::
open(boost::asio::ip::tcp::endpoint const& ep,
					boost::system::error_code& ec,
					Make_Func make,
					Args ...args)
{
	if(is_open(ep))
	{
		ec = boost::system::errc::make_error_code(boost::system::errc::address_in_use);
		return false;
	}

	if constexpr(!Session::use_ssl)
	{
		if constexpr(!SupportPlain)
		{
			ec = boost::system::errc::make_error_code(boost::system::errc::wrong_protocol_type);
			return false;
		} else {
			auto server = make(ioc_, ep, ec, args...);
			if(ec)
				return false;

			servers_[ep] = std::make_shared<Listener_Concept>(server);
		}
	} else {
		if constexpr(!SupportSSL)
		{
			ec = boost::system::errc::make_error_code(boost::system::errc::wrong_protocol_type);
			return false;
		} else {
#if USE_SSL == 1
			auto server = make(ioc_, ctx, ep, ec, args...);
			if(ec)
				return false;

			servers_[ep] = std::make_shared<Listener_Concept>(server);
#endif
		}
	}
	return true;
}

template<bool SupportSSL, bool SupportPlain>
template<typename OutContainer>
void Container<SupportSSL, SupportPlain>::
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
	auto const share_data = std::make_shared<Byte_Array const>(data);

	for(auto const& t : to)
		server->second->write(t, share_data);
}


template<bool SupportSSL, bool SupportPlain>
std::size_t
Container<SupportSSL, SupportPlain>::
count(){
	return servers_.size();
}

template<bool SupportSSL, bool SupportPlain>
bool
Container<SupportSSL, SupportPlain>::
is_open(boost::asio::ip::tcp::endpoint const& ep){ // @suppress("Member declaration not found")
	return servers_.find(ep) != servers_.end();
}

template<bool SupportSSL, bool SupportPlain>
void
Container<SupportSSL, SupportPlain>::
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

template<bool SupportSSL, bool SupportPlain>
client_list
Container<SupportSSL, SupportPlain>::
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

template<bool SupportSSL, bool SupportPlain>
void
Container<SupportSSL, SupportPlain>::
close_clients(boost::asio::ip::tcp::endpoint const& ep, client_list clients, boost::system::error_code& ec)
{
	auto server = servers_.find(ep);
	if(server == servers_.end())
	{
		ec = boost::system::errc::make_error_code(boost::system::errc::not_connected);
		return;
	}

	server->second->close_clients(clients);
}

template<bool SupportSSL, bool SupportPlain>
std::vector<std::pair<bool, boost::asio::ip::tcp::endpoint>>
Container<SupportSSL, SupportPlain>::
endpoints(){
	std::vector<std::pair<bool, boost::asio::ip::tcp::endpoint>> v;

	for (auto [key, value] : servers_) {
	    v.push_back({value->use_ssl(), key});
	}

	return v;
}

}//Listener
}//My_Async

#endif /* SESSION_LISTENER_TCP_CONTAINER_IMPL_HPP__ */
