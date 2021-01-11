#ifndef SESSION_LISTENER_HELPER_HPP__
#define SESSION_LISTENER_HELPER_HPP__

#include <boost/asio.hpp>
#if USE_SSL == 1
#include <boost/asio/ssl.hpp>
#endif

namespace My_Async{
namespace Listener{

template<class Session, bool MakeStrand = true>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ip::tcp::endpoint endpoint,
		boost::system::error_code&);

template<class Session, bool MakeStrand = true>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ip::address const& address,
		unsigned short int port,
		boost::system::error_code&);

#if USE_SSL == 1
template<class Session, bool MakeStrand = true>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ssl::context& ctx,
		boost::asio::ip::tcp::endpoint endpoint,
		boost::system::error_code&);

template<class Session, bool MakeStrand = true>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ssl::context& ctx,
		boost::asio::ip::address const& address,
		unsigned short int port,
		boost::system::error_code&);

template<class SessionPlain, class SessionSSL, bool MakeStrand = true>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ssl::context& ctx,
		boost::asio::ip::tcp::endpoint endpoint);

template<class SessionPlain, class SessionSSL, bool MakeStrand = true>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ssl::context& ctx,
		boost::asio::ip::address const& address,
		unsigned short int port);

template<template<bool> class Session, bool MakeStrand = true>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ssl::context& ctx,
		boost::asio::ip::tcp::endpoint endpoint);

template<template<bool> class Session, bool MakeStrand = true>
auto make_listener(
		boost::asio::io_context& ioc,
		boost::asio::ssl::context& ctx,
		boost::asio::ip::address const& address,
		unsigned short int port);
#endif

}//Listener
}//My_Async

#include "impl/listener_helper_impl.hpp"

#endif /* SESSION_LISTENER_HELPER_HPP__ */
