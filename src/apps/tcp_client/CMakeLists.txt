if((${USE_APP_TCP_CLIENT} EQUAL 2 OR 
	${USE_APP_TCP_CLIENT} EQUAL 3) AND 
	NOT (${USE_SSL} EQUAL 1))
	message(FATAL_ERROR "You must enable SSL support (USE_SSL=1) to use TCP Client SSL")
endif()

add_library(tcp_client_app tcp_client.cpp 
			json/make.cpp json/parser.cpp)

target_include_directories(tcp_client_app INTERFACE)

set_target_properties(tcp_client_app PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS ON
)


