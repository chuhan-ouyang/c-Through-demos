cmake_minimum_required(VERSION 3.10)
project(TCPBufferTest)

set(CMAKE_CXX_STANDARD 17)

# Add server executable
add_executable(server server.cpp)

# Add client executable
add_executable(client client.cpp)