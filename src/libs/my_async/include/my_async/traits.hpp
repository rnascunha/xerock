#ifndef MY_ASYNC_TYPE_TRAITS_HPP__
#define MY_ASYNC_TYPE_TRAITS_HPP__

//#include <type_traits>

namespace My_Async{

/**
 * Garante que não compila... mas não tá certo
 */
template<typename WebsocketSession>
constexpr bool is_websocket_session = WebsocketSession::websocket_session;


}//My_Async

#endif /* MY_ASYNC_TYPE_TRAITS_HPP__ */
