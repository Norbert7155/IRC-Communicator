cmake_minimum_required(VERSION 3.16)
project(IRCCommunicator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(chat_server server/main.cpp)
add_executable(chat_client client/main.cpp)
