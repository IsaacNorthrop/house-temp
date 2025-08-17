cmake_minimum_required(VERSION 3.10)
project(backend)

cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0167 NEW)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system filesystem serialization)
find_package(CURL REQUIRED)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find Boost libraries.")
endif()

if(NOT CURL_FOUND)
    message(FATAL_ERROR "Could not find CURL libraries.")
endif()

add_executable(backend main.cpp)

target_include_directories(backend PRIVATE
    ${Boost_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/json/include"
)

target_link_libraries(backend PRIVATE
    ${Boost_LIBRARIES}
    CURL::libcurl
)
