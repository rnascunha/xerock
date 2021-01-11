if(NOT WIN32)
    include_directories(../include)
    add_library(system_info proc_info_unix.cpp hardware_info_unix.cpp print_system.cpp system_util.cpp)

    target_link_libraries(system_info stdc++fs)
    set_target_properties(system_info PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS ON
    )
else()
    message(STATUS "System Info library not compiled [Windows sytem NOT supported]")
endif()