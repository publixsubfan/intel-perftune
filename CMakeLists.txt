cmake_minimum_required(VERSION 3.4)

project(intel-perftune LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(perftune
	src/mailbox.cpp
	src/main.cpp)