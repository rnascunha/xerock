#ifndef UDP_CLIENT_SIMPLE_CONTAINER_IMPL_HPP__
#define UDP_CLIENT_SIMPLE_CONTAINER_IMPL_HPP__

namespace My_Async{
namespace UDP{

template<typename Client>
template<typename ...Args>
void
Simple_Container<Client>::
open(boost::asio::ip::udp::endpoint const& ep, Args... args)
{
	auto client = std::make_shared<Client>(ioc_, std::forward<Args&&>(args)...);
	client->open(ep);
	clients_[client->local_endpoint()] = std::move(client);
}

template<typename Client>
void Simple_Container<Client>::
close(boost::asio::ip::udp::endpoint const& local_ep,
		boost::system::error_code& ec)
{
	auto client = clients_.find(local_ep);
	if(client == clients_.end())
	{
		ec = boost::system::errc::make_error_code(boost::system::errc::not_connected);
		return;
	}

	client->second->close(ec);
	clients_.erase(client);
}

template<typename Client>
template<typename OutContainer>
void
Simple_Container<Client>::
write(boost::asio::ip::udp::endpoint const& ep,
			OutContainer const data,
			boost::system::error_code& ec)
{
	auto client = clients_.find(ep);
	if(client == clients_.end())
	{
		ec = boost::system::errc::make_error_code(boost::system::errc::not_connected);
		return;
	}

	client->second->write(std::make_shared<OutContainer const>(data));
}

template<typename Client>
std::optional<std::shared_ptr<Client>>
Simple_Container<Client>::
client(boost::asio::ip::udp::endpoint const& local_ep)
{

	auto client = clients_.find(local_ep);
	if(client != clients_.end())
		return client->second;

	return std::nullopt;
}

template<typename Client>
std::vector<std::pair<boost::asio::ip::udp::endpoint, boost::asio::ip::udp::endpoint>>
Simple_Container<Client>::
endpoints()
{
	std::vector<std::pair<boost::asio::ip::udp::endpoint, boost::asio::ip::udp::endpoint>> v;

	for (auto const& [ep, socket] : clients_)
	    v.push_back({socket->local_endpoint(), socket->endpoint()});

	return v;
}

template<typename Client>
std::size_t
Simple_Container<Client>::
size()
{
	return clients_.size();
}

}//UDP
}//My_Async

#endif /* UDP_CLIENT_SIMPLE_CONTAINER_IMPL_HPP__ */
