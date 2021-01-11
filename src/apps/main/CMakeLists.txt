include_directories(.)
add_library(main_app 
			json/parser.cpp  
			json/make.cpp)

set_target_properties(main_app PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

target_link_libraries(main_app utility my_async byte_array)
