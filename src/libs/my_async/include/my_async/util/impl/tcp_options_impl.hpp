#ifndef MY_ASYNC_TCP_OPTIONS_IMPL_HPP__
#define MY_ASYNC_TCP_OPTIONS_IMPL_HPP__

namespace My_Async{
namespace TCP{

template<typename Socket>
void keepalive(Socket const& socket, int32_t idle, int32_t count, int32_t interval) noexcept
{
#ifdef __unix__
	::setsockopt(socket, SOL_TCP, TCP_KEEPIDLE, &idle, sizeof(int32_t));
	::setsockopt(socket, SOL_TCP, TCP_KEEPCNT, &count, sizeof(int32_t));
	::setsockopt(socket, SOL_TCP, TCP_KEEPINTVL, &interval, sizeof(int32_t));
#else /* __unix__ */
#	if KEEPALIVE_OLD == 0
	::setsockopt(socket, IPPROTO_TCP, TCP_KEEPIDLE, &static_cast<const char>(idle), sizeof(int32_t));
	::setsockopt(socket, IPPROTO_TCP, TCP_KEEPCNT, &static_cast<const char>(count), sizeof(int32_t));
	::setsockopt(socket, IPPROTO_TCP, TCP_KEEPINTVL, &static_cast<const char>(interval), sizeof(int32_t));
#else /* KEEPALIVE_OLD == 0 */
	DWORD bytes_ret = 0;
	tcp_keepalive kpalive;

	kpalive.onoff = 1;
	kpalive.keepaliveinterval = interval * 1000;	//Miliseconds
	kpalive.keepalivetime = idle * 1000;			//Miliseconds

	WSAIoctl(socket, SIO_KEEPALIVE_VALS, &kpalive, sizeof(kpalive), NULL, 0, &bytes_ret, NULL, NULL);
#endif /* KEEPALIVE_OLD == 0 */
#endif /* __unix__ */
}

}//TCP
}//My_Async

#endif /* MY_ASYNC_TCP_OPTIONS_IMPL_HPP__ */
