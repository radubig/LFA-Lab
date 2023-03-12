cmake_minimum_required(VERSION 3.20)
project(Tema_Lab_1)

set(CMAKE_CXX_STANDARD 14)

add_executable(NFA NFA.cpp)
add_executable(DFA DFA.cpp)
add_executable(L_NFA lambda-NFA.cpp)
