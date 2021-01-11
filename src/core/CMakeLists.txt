add_library(core 
			app.cpp 
			error.cpp 
			functions.cpp
			json/factory.cpp
			json/websocket_session.cpp)
			
set_target_properties(core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

target_link_libraries(core utility my_async byte_array)
