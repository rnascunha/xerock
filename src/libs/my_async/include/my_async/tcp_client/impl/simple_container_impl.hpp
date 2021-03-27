#ifndef TCP_CLIENT_SIMPLE_CONTAINER_IMPL_HPP__
#define TCP_CLIENT_SIMPLE_CONTAINER_IMPL_HPP__

namespace My_Async{
namespace TCP{

template<typename Client>
template<typename Callback, typename ...Args>
void
Simple_Container<Client>::
open(boost::asio::ip::tcp::endpoint const& ep,
		Callback cb, Args... args)
{
	if constexpr(Client::use_ssl)
	{	//SSL
#if USE_SSL == 1
		auto client = std::make_shared<Client>(ioc_, ctx_, std::forward<Args>(args)...);
		boost::asio::post(ioc_, [&, client, ep, cb]{
			boost::system::error_code ec;

			client->open(ep, ec);

			cb(client, ec);
			if(ec) return;

			clients_[client->local_endpoint()] = std::move(client);
		});
#endif
	}
	else
	{	//Plain
		auto client = std::make_shared<Client>(ioc_, std::forward<Args>(args)...);
		boost::asio::post(ioc_, [&, client, ep, cb]{
			boost::system::error_code ec;

			client->open(ep, ec);

			cb(client, ec);
			if(ec) return;

			clients_[client->local_endpoint()] = std::move(client);
		});
	}
}

template<typename Client>
template<typename ...Args>
void
Simple_Container<Client>::
open(boost::asio::ip::tcp::endpoint const& ep, Args... args)
{
	open(ep, [](std::shared_ptr<Client>, boost::system::error_code){}, std::forward<Args>(args)...);
}

template<typename Client>
void Simple_Container<Client>::
close(boost::asio::ip::tcp::endpoint const& local_ep,
		boost::system::error_code& ec)
{
	auto client = clients_.find(local_ep);
	if(client == clients_.end())
	{
		ec = boost::system::errc::make_error_code(boost::system::errc::not_connected);
		return;
	}

	client->second->close();
	clients_.erase(client);
}

template<typename Client>
template<typename OutContainer>
void
Simple_Container<Client>::
write(boost::asio::ip::tcp::endpoint const& ep,
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
client(boost::asio::ip::tcp::endpoint const& local_ep)
{

	auto client = clients_.find(local_ep);
	if(client != clients_.end())
		return client->second;

	return std::nullopt;
}

template<typename Client>
std::vector<std::pair<boost::asio::ip::tcp::endpoint, boost::asio::ip::tcp::endpoint>>
Simple_Container<Client>::
endpoints()
{
	std::vector<std::pair<boost::asio::ip::tcp::endpoint, boost::asio::ip::tcp::endpoint>> v;

	for (auto&&  c : clients_)
	    v.push_back({c.local_endpoint(), c.remote_endpoint()});

	return v;
}

template<typename Client>
std::size_t
Simple_Container<Client>::
size()
{
	return clients_.size();
}

}//TCP
}//My_Async

#endif /* TCP_CLIENT_SIMPLE_CONTAINER_IMPL_HPP__ */
