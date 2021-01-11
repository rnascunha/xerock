#ifndef INIT_APPS_HPP__
#define INIT_APPS_HPP__

#include <boost/asio.hpp>

#if USE_SSL == 1
#include <boost/asio/ssl.hpp>
#endif /* USE_SSL == 1 */

namespace Apps{

#if USE_SSL == 1
void init_apps(boost::asio::io_context& ioc, boost::asio::ssl::context& ctx);
#else
void init_apps(boost::asio::io_context& ioc);
#endif /* USE_SSL == 1 */

}//Apps

#endif /* INIT_APPS_HPP__ */
