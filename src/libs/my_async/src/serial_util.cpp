#include "my_async/serial/serial_util.hpp"

#include <vector>
#include <algorithm>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#	include <windows.h>
#elif defined(__unix__)
#	include <dirent.h>
#else
#	error "Library not defined for your plataform"
#endif /* WIN32 */


namespace My_Async{
namespace Serial{

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)

#define BUFFER_SIZE 256
#define UPPER_LIMIT 128

std::vector<std::string> scan_ports()
{
	std::vector<std::string> ports;
	char buffer[BUFFER_SIZE];
	std::ostringstream port;

	for(unsigned int i = 0; i < UPPER_LIMIT; i++){
		port.str("");port.clear();
		port << "COM" << i;

		unsigned int result = QueryDosDevice(port.str().c_str(),buffer,BUFFER_SIZE);

		if(result == 0){ /* error */
			DWORD err = ::GetLastError();
			if(err == ERROR_INSUFFICIENT_BUFFER)
				break;
			else{ /* Not found */
				continue;
			}
		}
		ports.push_back(port.str());
	}

	return ports;
}

#elif defined(__unix__)

#define DEVICE_DIR		"/dev/"
#define SERIAL_PREFIX	"ttyUSB","ttyACM"

std::vector<std::string> scan_ports()
{
	std::vector<std::string> ports;
	const std::vector<const char*> base_dir{DEVICE_DIR};
	const std::vector<const char*> serial_port_id{SERIAL_PREFIX};

	for(auto b : base_dir)
	{
		DIR* dirp = opendir(b);
		if(!dirp)
			continue;

		struct dirent *dp;
		while ((dp = readdir(dirp)) != NULL)
		{
			std::string port(dp->d_name);
			for(auto p : serial_port_id){
				if(port.find(p) == 0){
					ports.push_back(std::string(b) + dp->d_name);
					break;
				}
			}
		}
		(void)closedir(dirp);
	}
	return ports;
}

#endif

}//Serial
}//My_Async
