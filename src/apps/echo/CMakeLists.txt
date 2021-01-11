add_library(echo_app json/echo.cpp json/parser.cpp)

set_target_properties(echo_app PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

target_link_libraries(echo_app utility)