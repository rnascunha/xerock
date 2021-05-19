#ifndef TCP_CLIENT_CONTAINER_IMPL_HPP__
#define TCP_CLIENT_CONTAINER_IMPL_HPP__

namespace My_Async{
namespace TCP{

template<bool SupportSSL, bool SupportPlain>
template<typename Client, typename Callback, typename ...Args>
void
Container<SupportSSL, SupportPlain>::
open(boost::asio::ip::tcp::endpoint const& ep,
		boost::system::error_code& ec,
		Callback cb, Args... args)
{
	if constexpr(Client::use_ssl)
	{	//SSL
		if constexpr (!SupportSSL)
		{
			ec = boost::system::errc::make_error_code(boost::system::errc::wrong_protocol_type);
			return;
		} else {
#if USE_SSL == 1
			auto client = std::make_shared<Client>(ioc_, ctx_, std::forward<Args>(args)...);
			client->open(ep, [&, cb](std::shared_ptr<Client> client, const boost::system::error_code& ec){
				if(!ec)
					clients_[client->local_endpoint()] = std::make_shared<Client_Concept>(client);

				cb(client, ec);
			});
#endif
		}
	}
	else
	{	//Plain
		if constexpr(!SupportPlain)
		{
			ec = boost::system::errc::make_error_code(boost::system::errc::wrong_protocol_type);
			return;
		} else {
			auto client = std::make_shared<Client>(ioc_, std::forward<Args>(args)...);
			client->open(ep, [&, cb](std::shared_ptr<Client> client, const boost::system::error_code& ec){
				if(!ec)
					clients_[client->local_endpoint()] = std::make_shared<Client_Concept>(client);

				cb(client, ec);
			});
		}
	}
}

template<bool SupportSSL, bool SupportPlain>
template<typename Client, typename ...Args>
void
Container<SupportSSL, SupportPlain>::
open(boost::asio::ip::tcp::endpoint const& ep,
		boost::system::error_code& ec,
		Args... args)
{
	open<Client>(ep, ec, [](std::shared_ptr<Client>, boost::system::error_code){}, std::forward<Args>(args)...);
}

template<bool SupportSSL, bool SupportPlain>
void
Container<SupportSSL, SupportPlain>::
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

template<bool SupportSSL, bool SupportPlain>
template<typename OutContainer>
void
Container<SupportSSL, SupportPlain>::
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

template<bool SupportSSL, bool SupportPlain>
std::vector<std::tuple<bool, boost::asio::ip::tcp::endpoint, boost::asio::ip::tcp::endpoint>>
Container<SupportSSL, SupportPlain>::
endpoints()
{
	std::vector<std::tuple<bool, boost::asio::ip::tcp::endpoint, boost::asio::ip::tcp::endpoint>> v;

	for (auto&&  [key, value] : clients_)
	    v.push_back(std::make_tuple(value->use_ssl(), value->local_endpoint(), value->endpoint()));

	return v;
}

template<bool SupportSSL, bool SupportPlain>
std::size_t
Container<SupportSSL, SupportPlain>::
size()
{
	return clients_.size();
}

}//TCP
}//My_Async

#endif /* TCP_CLIENT_CONTAINER_IMPL_HPP__ */
