add_library(serial_app serial.cpp serial_type.cpp 
						json/make.cpp json/parser.cpp)
set_target_properties(serial_app PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)
target_link_libraries(serial_app utility byte_array my_async)