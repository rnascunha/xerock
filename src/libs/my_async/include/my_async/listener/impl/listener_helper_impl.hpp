#ifndef SESSION_LISTENER_HELPER_IMPL_HPP__
#define SESSION_LISTENER_HELPER_IMPL_HPP__

#include <memory>

#include "my_async/listener/listener_base.hpp"
#if USE_SSL == 1
#include "my_async/listener/listener_dual.hpp"
#endif

namespace My_Async{
namespace Listener{

template<class Session, bool MakeStrand /* = true */>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ip::tcp::endpoint endpoint,
    	boost::system::error_code& ec)
{
	static_assert(Session::use_ssl == false,
			"Session must be plain type");

	auto l = std::make_shared<My_Async::Listener::
			Listener_Base<Session, MakeStrand>>(ioc);

	l->init(endpoint, ec);
	return l;
}

template<class Session, bool MakeStrand /* = true */>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ip::address const& address,
		unsigned short int port,
    	boost::system::error_code& ec)
{
	static_assert(Session::use_ssl == false,
			"Session must be plain type");

	auto l = std::make_shared<My_Async::Listener::
			Listener_Base<Session, MakeStrand>>(ioc);

	l->init(boost::asio::ip::tcp::endpoint{address, port}, ec);
	return l;
}

#if USE_SSL == 1
template<class Session, bool MakeStrand /* = true */>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ssl::context& ctx,
		boost::asio::ip::tcp::endpoint endpoint,
    	boost::system::error_code& ec)
{
	static_assert(Session::use_ssl == true,
			"Session must be SSL type");

	auto l = std::make_shared<My_Async::Listener::
			Listener_Base<Session, MakeStrand>>
					(ioc, ctx);

	l->init(endpoint, ec);
	return l;
}

template<class Session, bool MakeStrand /* = true */>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ssl::context& ctx,
		boost::asio::ip::address const& address,
		unsigned short int port,
    	boost::system::error_code& ec)
{
	static_assert(Session::use_ssl == true,
			"Session must be SSL type");

	auto l = std::make_shared<My_Async::Listener::
			Listener_Base<Session, MakeStrand>>
					(ioc, ctx);

	l->init(boost::asio::ip::tcp::endpoint{address, port}, ec);
	return l;
}

template<class SessionPlain, class SessionSSL, bool MakeStrand /* = true */>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ssl::context& ctx,
		boost::asio::ip::tcp::endpoint endpoint)
{
	static_assert(SessionPlain::use_ssl == false
			&& SessionSSL::use_ssl == true,
			"SessionPlain must be plain and SessionSSL ssl type");

	return std::make_shared<My_Async::Listener::
			Listener_Dual<SessionPlain, SessionSSL, MakeStrand>>
					(ioc,
					ctx,
					endpoint);
}

template<class SessionPlain, class SessionSSL, bool MakeStrand /* = true */>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ssl::context& ctx,
		boost::asio::ip::address const& address,
		unsigned short int port)
{
	static_assert(SessionPlain::use_ssl == false
			&& SessionSSL::use_ssl == true,
			"SessionPlain must be plain and SessionSSL ssl type");

	return std::make_shared<My_Async::Listener::
			Listener_Dual<SessionPlain, SessionSSL, MakeStrand>>
					(ioc,
					ctx,
					boost::asio::ip::tcp::endpoint{address, port});
}

template<template<bool> class Session, bool MakeStrand /* = true */>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ssl::context& ctx,
		boost::asio::ip::tcp::endpoint endpoint){
	return make_listener<Session<false>,Session<true>, MakeStrand>(ioc, ctx, endpoint);
}


template<template<bool> class Session, bool MakeStrand /* = true */>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ssl::context& ctx,
		boost::asio::ip::address const& address,
		unsigned short int port){
	return make_listener<Session<false>,Session<true>, MakeStrand>(ioc, ctx, address, port);
}
#endif

}//Listener
}//My_Async

#endif /* SESSION_LISTENER_HELPER_IMPL_HPP__ */
