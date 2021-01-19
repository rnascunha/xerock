cmake_minimum_required (VERSION 3.10)
project (xerock)

function(print_vars)	
	message("USE_SSL: " ${USE_SSL})
	message("USE_APP_ECHO: " ${USE_APP_ECHO})
	message("USE_APP_MONITOR: " ${USE_APP_MONITOR})
	message("USE_APP_TCP_SERVER: " ${USE_APP_TCP_SERVER})
	message("USE_APP_TCP_CLIENT: " ${USE_APP_TCP_CLIENT}) 
	message("BOOST_ROOT: " ${BOOST_ROOT})
	message("RAPIDJSON_ROOT: " ${RAPIDJSON_ROOT})
	message("ARGH_ROOT: " ${ARGH_ROOT})
	message("OPENSSL_ROOT: " ${OPENSSL_ROOT})
	message("KEEPALIVE_OLD: " ${KEEPALIVE_OLD})
endfunction()

#------------DEFINING VARS-----------------

#
#Setting defualt values
#
#0 - disable / 1 - enable
set(USE_SSL 			0)
set(USE_APP_ECHO 		1)
set(USE_APP_SERIAL 		1)
set(USE_APP_MONITOR 	1)

#0 - disable
#1 - enable plain
#2 - enble ssl
#3 - enable plain & ssl
set(USE_APP_TCP_SERVER 	1)
set(USE_APP_TCP_CLIENT 	1)

set(BOOST_ROOT 			third/boost)
set(RAPIDJSON_ROOT 		third/rapidjson/include)
set(ARGH_ROOT 			third/argh)

#print_vars()

if(DEFINED WITH_SSL)
	if(${WITH_SSL} EQUAL 1)
		set(USE_SSL 1)
	endif()
	unset(WITH_SSL CACHE)
endif()

if(DEFINED APP_ECHO)
	if(${APP_ECHO} EQUAL 0)
		set(USE_APP_ECHO 0)
	endif()
	unset(APP_ECHO CACHE)
endif()

if(DEFINED APP_SERIAL)
	if(${APP_SERIAL} EQUAL 0)
		set(USE_APP_SERIAL 0)
	endif()
	unset(APP_SERIAL CACHE)
endif()

if(DEFINED APP_MONITOR)
	if(${APP_MONITOR} EQUAL 0)
		set(USE_APP_MONITOR 0)
	elseif(${APP_MONITOR} EQUAL 1 AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
		message(FATAL_ERROR "APP Monitor isn't supported at Windows. Disable (-DAPP_MONITOR=0) or just doesn't explicitly enable.")
	endif()
	unset(APP_MONITOR CACHE)
endif()

if(DEFINED APP_TCP_SERVER)
	if(${APP_TCP_SERVER} GREATER_EQUAL 0 AND ${APP_TCP_SERVER} LESS_EQUAL 3)
		set(USE_APP_TCP_SERVER ${APP_TCP_SERVER})
	else()
		message(FATAL_ERROR "-DAPP_TCP_SERVER must be a value >= 0 OR <= 3")
	endif()
	if(${USE_APP_TCP_SERVER} GREATER_EQUAL 2 AND NOT ${USE_SSL} EQUAL 1)
		message(FATAL_ERROR "TCP Server with support to SSL, must enable SSL globally (-DWITH_SSL=1).")
	endif()
	unset(APP_TCP_SERVER CACHE)
endif()

if(DEFINED APP_TCP_CLIENT)
	if(${APP_TCP_CLIENT} GREATER_EQUAL 0 AND ${APP_TCP_CLIENT} LESS_EQUAL 3)
		set(USE_APP_TCP_CLIENT ${APP_TCP_CLIENT})
	else()
		message(FATAL_ERROR "-DAPP_TCP_CLIENT must be a value >= 0 OR <= 3")
	endif()
	if(${USE_APP_TCP_CLIENT} GREATER_EQUAL 2 AND NOT ${USE_SSL} EQUAL 1)
		message(FATAL_ERROR "TCP Client with support to SSL, must enable SSL globally (-DWITH_SSL=1).")
	endif()
	unset(APP_TCP_CLIENT CACHE)
endif()

if(DEFINED BOOST_PATH)
	set(BOOST_ROOT ${BOOST_PATH})
	unset(BOOST_PATH CACHE)
endif()

if(DEFINED RAPIDJSON_PATH)
	set(RAPIDJSON_ROOT ${RAPIDJSON_PATH})
	unset(BOOST_PATH CACHE)
endif()

if(DEFINED ARGH_PATH)
	set(ARGH_ROOT ${ARGH_PATH})
	unset(ARGH_ROOT CACHE)
endif()

if(DEFINED OPENSSL_PATH)
	set(OPENSSL_ROOT_DIR ${OPENSSL_PATH})
	unset(OPENSSL_PATH CACHE)
endif()

#This flag works only on Windows OS. There is 2 implementations (that I call OLD or NEW).
#You can force the implementation with the -DCONFIG_KEEPALIVE_OLD=<0|1>, but if not defined MSVC will 
#use the NEW implementation, otherwise the OLD. This was defined by some tests (and no better
#defintion)
if(DEFINED CONFIG_KEEPALIVE_OLD)
	set(KEEPALIVE_OLD ${CONFIG_KEEPALIVE_OLD})
else()
	if(MSVC)
		set(KEEPALIVE_OLD 0)
	else()
		set(KEEPALIVE_OLD 1)
	endif()
endif()
unset(CONFIG_KEEPALIVE_OLD CACHE)

#print_vars()

#------------END DEFINING VARS-----------------

message(STATUS "Building OS: " ${CMAKE_SYSTEM_NAME})

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

message("Builder type: " ${CMAKE_BUILD_TYPE}) 

if(MSVC)
	message(STATUS "MSVC build")
	set(CMAKE_CXX_FLAGS "/W4 /std:c++17 /EHsc /bigobj")
	set(CMAKE_CXX_FLAGS_RELEASE "/O2")
	add_compile_definitions(BOOST_DATE_TIME_NO_LIB BOOST_REGEX_NO_LIB)
else()
	message(STATUS "NO MSVC build")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter")	#-fmax-errors=5 
	set(CMAKE_CXX_FLAGS_DEBUG "-g")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

if(WIN32)
    macro(get_WIN32_WINNT version)
        if(CMAKE_SYSTEM_VERSION)
            set(ver ${CMAKE_SYSTEM_VERSION})
            string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
            string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
            # Check for Windows 10, b/c we'll need to convert to hex 'A'.
            if("${verMajor}" MATCHES "10")
                set(verMajor "A")
                string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
            endif()
            # Remove all remaining '.' characters.
            string(REPLACE "." "" ver ${ver})
            # Prepend each digit with a zero.
            string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
            set(${version} "0x${ver}")
        endif()
    endmacro()

    get_WIN32_WINNT(ver)
    add_definitions(-D_WIN32_WINNT=${ver})
        message("WIN32: " ${ver})
endif()

message(STATUS "Compiler: " ${CMAKE_CXX_COMPILER_ID} " " ${CMAKE_CXX_COMPILER_VERSION})

#APP Monitor just support Linux system
if(USE_APP_MONITOR EQUAL 1 AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	message(STATUS "App Monitor doesn\'t support Windows system. Removing.")
	set(USE_APP_MONITOR 0)
endif()

add_definitions(-DBOOST_SYSTEM_USE_UTF8		#Make windows error output UTF8 / Ignored at POSIX
				-DJSON_MESSAGES 
				-DUSE_APP_ECHO=${USE_APP_ECHO}
				-DUSE_APP_SERIAL=${USE_APP_SERIAL}
				-DUSE_APP_MONITOR=${USE_APP_MONITOR}
				-DUSE_APP_TCP_SERVER=${USE_APP_TCP_SERVER}
				-DUSE_APP_TCP_CLIENT=${USE_APP_TCP_CLIENT}
				-DUSE_SSL=${USE_SSL}
				-DKEEPALIVE_OLD=${KEEPALIVE_OLD})

find_package(Boost COMPONENTS REQUIRED)

if(Boost_FOUND)
	message(STATUS "Boost Library found!")
	include_directories(${Boost_INCLUDE_DIRS})
else()
	message(FATAL_ERROR "Boost Library NOT found! Define variable BOOST_ROOT (and BOOST_INCLUDEDIR if necessary")
endif()

if(USE_SSL)
	find_package(OpenSSL REQUIRED)
	if(OPENSSL_FOUND)
		message(STATUS "OpenSSL Libraty found!")
		include_directories(${OPENSSL_INCLUDE_DIR})
	else()
		message(FATAL_ERROR "OpenSSL Libraty NOT found! Define variable_requires OPENSSL_ROOT_DIR, or compile WITHOUT SSL support")
	endif()
else()
	message(STATUS "Compiling WITHOUT SSL support")
endif()

set(SRC_DIR ./src)

set(MY_LIBS_DIR	${SRC_DIR}/libs)

include_directories(${RAPIDJSON_ROOT}
					${MY_LIBS_DIR}/byte_array/include 
					${MY_LIBS_DIR}/my_async/include
					${MY_LIBS_DIR}/utility/include
					${MY_LIBS_DIR}/system_info/include
					${SRC_DIR}/helpers/include
					${SRC_DIR}/core/include
					${ARGH_ROOT})								

add_subdirectory(${MY_LIBS_DIR})

set(APPS_DIR ${SRC_DIR}/apps)

if(${USE_SSL} EQUAL 1)
	message("Enable SSL support")
endif()

message("Adding Main APP")
add_subdirectory(${APPS_DIR}/main)

if(${USE_APP_ECHO} EQUAL 1)
	message("Enable App Echo")
	add_subdirectory(${APPS_DIR}/echo)
endif()
if(${USE_APP_SERIAL} EQUAL 1)
	message("Enable App Serial")
	add_subdirectory(${APPS_DIR}/serial)
endif()
if(${USE_APP_MONITOR} EQUAL 1)
	message("Enable App Monitor")
	add_subdirectory(${APPS_DIR}/monitor)
endif()
if(${USE_APP_TCP_SERVER} GREATER 0)
	message("Enable App TCP Server " ${USE_APP_TCP_SERVER})
	add_subdirectory(${APPS_DIR}/tcp_server)
endif()
if(${USE_APP_TCP_CLIENT} GREATER 0)
	message("Enable App TCP Client " ${USE_APP_TCP_CLIENT})
	add_subdirectory(${APPS_DIR}/tcp_client)
endif()

message("Adding Core")
add_subdirectory(${SRC_DIR}/core)

set(MAIN_SRC ${SRC_DIR}/main.cpp 
				${SRC_DIR}/arguments.cpp 
				${SRC_DIR}/apps/init_apps.cpp)

if(USE_SSL)
	list(APPEND MAIN_SRC ${SRC_DIR}/helpers/load_certificate.cpp)
endif()

add_executable(${PROJECT_NAME} ${MAIN_SRC})
										
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

if(USE_SSL)
	target_link_libraries(${PROJECT_NAME} OpenSSL::SSL)
endif()

target_link_libraries(${PROJECT_NAME} byte_array)
target_link_libraries(${PROJECT_NAME} utility)
target_link_libraries(${PROJECT_NAME} my_async)

target_link_libraries(${PROJECT_NAME} core)
target_link_libraries(${PROJECT_NAME} main_app)

if(${USE_APP_ECHO} EQUAL 1)
	target_link_libraries(${PROJECT_NAME} echo_app)
endif()
if(${USE_APP_SERIAL} EQUAL 1)
	target_link_libraries(${PROJECT_NAME} serial_app)
endif()
if(${USE_APP_MONITOR} EQUAL 1)
	target_link_libraries(${PROJECT_NAME} monitor_app)
endif()
if(${USE_APP_TCP_SERVER} GREATER 0)
	target_link_libraries(${PROJECT_NAME} tcp_server_app)
endif()
if(${USE_APP_TCP_CLIENT} GREATER 0)
	target_link_libraries(${PROJECT_NAME} tcp_client_app)
endif()

if(WIN32)
	target_link_libraries(${PROJECT_NAME} wsock32 ws2_32)
else()
	target_link_libraries(${PROJECT_NAME} pthread)
endif()
