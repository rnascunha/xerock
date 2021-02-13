add_library(udp_server_app udp_server.cpp udp_server_type.cpp
			json/make.cpp json/parser.cpp)

target_include_directories(udp_server_app INTERFACE)

set_target_properties(udp_server_app PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS ON
)


