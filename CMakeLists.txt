cmake_minimum_required(VERSION 3.15)
project(lab_17_2)

set(CMAKE_CXX_STANDARD 17)

#set(BADS "-Werror -Wall -Wextra -pedantic")

set(FLAGS "-pthread")

SET(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} ${FLAGS} ${BADS}")

include_directories(include)

add_executable(lab_17_2 src/main.cpp include/sync_queue.hpp include/mainHelper.hpp src/mainHelper.cpp src/threadHelper.cpp)
