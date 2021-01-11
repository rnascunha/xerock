include_directories(../include ${Boost_INCLUDE_DIRS})
add_library(my_async serial_util.cpp)

set_target_properties(my_async PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)
