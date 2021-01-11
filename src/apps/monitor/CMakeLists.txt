add_library(monitor_app executor.cpp monitor.cpp json/make.cpp json/parser.cpp)
set_target_properties(monitor_app PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

target_link_libraries(monitor_app system_info)