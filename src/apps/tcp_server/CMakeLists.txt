if((${USE_APP_TCP_SERVER} EQUAL 2 OR 
	${USE_APP_TCP_SERVER} EQUAL 3) AND 
	NOT (${USE_SSL} EQUAL 1))
	message(FATAL_ERROR "You must enable SSL support (USE_SSL=1) to use TCP Server SSL")
endif()

add_library(tcp_server_app tcp_server.cpp init_cb.cpp share_data.cpp 
			json/make.cpp json/parser.cpp)
target_include_directories(tcp_server_app INTERFACE)
set_target_properties(tcp_server_app PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS ON
)


