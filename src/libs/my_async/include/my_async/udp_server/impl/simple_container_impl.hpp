#ifndef UDP_SERVER_SIMPLE_CONTAINER_IMPL_HPP__
#define UDP_SERVER_SIMPLE_CONTAINER_IMPL_HPP__

namespace My_Async{
namespace UDP{

template<typename Server>
template<typename ...Args>
void
Simple_Container_Server<Server>::
open(boost::asio::ip::udp::endpoint const& ep, boost::system::error_code& ec, Args... args)
{
	auto server = std::make_shared<Server>(ioc_, std::forward<Args&&>(args)...);
	server->open(ep, ec);
	if(ec) return;

	servers_[server->local_endpoint()] = server;
}

template<typename Server>
void Simple_Container_Server<Server>::
close(boost::asio::ip::udp::endpoint const& local_ep,
		boost::system::error_code& ec)
{
	auto server = servers_.find(local_ep);
	if(server == servers_.end())
	{
		ec = boost::system::errc::make_error_code(boost::system::errc::not_connected);
		return;
	}

	server->second->close(ec);
	servers_.erase(server);
}

template<typename Server>
template<typename OutContainer>
void
Simple_Container_Server<Server>::
write(boost::asio::ip::udp::endpoint const& from,
		boost::asio::ip::udp::endpoint const& to,
		OutContainer const& data,
		boost::system::error_code& ec)
{
	auto server = servers_.find(from);
	if(server == servers_.end())
	{
		ec = boost::system::errc::make_error_code(boost::system::errc::not_connected);
		return;
	}

	server->second->write(to, std::make_shared<OutContainer const>(data));
}

template<typename Server>
template<typename OutContainer>
void
Simple_Container_Server<Server>::
write(boost::asio::ip::udp::endpoint const& from,
		std::vector<boost::asio::ip::udp::endpoint> const& tos,
		OutContainer const& data,
		boost::system::error_code& ec)
{
	auto server = servers_.find(from);
	if(server == servers_.end())
	{
		ec = boost::system::errc::make_error_code(boost::system::errc::not_connected);
		return;
	}

	auto const s_data = std::make_shared<OutContainer const>(data);
	for(auto const& to : tos)
		server->second->write(to, s_data);
}

template<typename Server>
std::optional<std::shared_ptr<Server>>
Simple_Container_Server<Server>::
server(boost::asio::ip::udp::endpoint const& local_ep)
{

	auto server = servers_.find(local_ep);
	if(server != servers_.end())
		return server->second;

	return std::nullopt;
}

template<typename Server>
std::vector<boost::asio::ip::udp::endpoint>
Simple_Container_Server<Server>::
endpoints()
{
	std::vector<boost::asio::ip::udp::endpoint> v;

	for (auto const [ep, server] : servers_)
	    v.push_back(ep);

	return v;
}

template<typename Server>
std::size_t
Simple_Container_Server<Server>::
size()
{
	return servers_.size();
}

}//UDP
}//My_Async

#endif /* UDP_SERVER_SIMPLE_CONTAINER_IMPL_HPP__ */
