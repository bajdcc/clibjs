cmake_minimum_required(VERSION 3.12)
project(clibjs)

set(CMAKE_CXX_STANDARD 14)

add_executable(clibjs
        main.cpp
        cjs.cpp
        cjs.h
        cjstypes.cpp
        cjstypes.h
        cjsmem.cpp
        cjsmem.h
        cjsast.cpp
        cjsast.h
        cjsunit.cpp
        cjsunit.h
        cjslexer.cpp
        cjslexer.h
        cjsparser.cpp
        cjsparser.h
        cjsgen.cpp
        cjsgen.h
        cjsgen_ast.cpp)