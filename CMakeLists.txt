cmake_minimum_required(VERSION 3.10)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 14)

project(YX)

# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

include_directories(include "/usr/include/mysql" three)

add_executable(yx main.cpp MysqlConn.cpp ConnectionPool.cpp three/cJSON.c)

target_link_libraries(yx mysqlclient)

target_link_libraries(yx pthread)