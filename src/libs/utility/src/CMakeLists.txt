include_directories(../include)
add_library(utility handler_helper.cpp json_helper.cpp)

set_target_properties(utility PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)