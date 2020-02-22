cmake_minimum_required(VERSION 3.12)
project(clibjs)

set(CMAKE_CXX_STANDARD 14)

add_executable(clibjs
        main.cpp
        cjs.cpp
        cjs.h
        cjslexer.cpp
        cjslexer.h
        cjsparser.cpp
        cjsparser.h
        cjsnode.cpp)