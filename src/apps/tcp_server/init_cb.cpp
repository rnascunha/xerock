#include "tcp_server.hpp"
#include "impl/init_cb_impl.hpp"

namespace Apps{
namespace TCP_Server{

void init_cb(std::shared_ptr<TCP_Container> servers)
{
#if USE_APP_TCP_SERVER == 1 || USE_APP_TCP_SERVER == 3
    init_cb2<false>(servers);	//Plain
#endif /* USE_APP_TCP_SERVER == 1 || ${USE_APP_TCP_SERVER} == 3 */
#if USE_APP_TCP_SERVER == 2 || USE_APP_TCP_SERVER == 3
    init_cb2<true>(servers);	//SSL
#endif /* USE_APP_TCP_SERVER == 1 || ${USE_APP_TCP_SERVER} == 3 */
}

}//TCP_Server
}//Apps
