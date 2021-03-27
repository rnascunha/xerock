add_library(udp_client_app udp_client.cpp 
			json/make.cpp json/parser.cpp)

target_include_directories(udp_client_app INTERFACE)

set_target_properties(udp_client_app PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS ON
)


