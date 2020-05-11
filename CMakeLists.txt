cmake_minimum_required(VERSION 3.12)
project(clibjs)

set(CMAKE_CXX_STANDARD 14)

add_executable(clibjs
        main.cpp
        cjs.cpp
        cjstypes.cpp
        cjsmem.cpp
        cjsast.cpp
        cjsunit.cpp
        cjslexer.cpp
        cjsparser.cpp
        cjsgen.cpp
        cjsgen_ast.cpp
        cjsruntime.cpp
        cjsruntime_base.cpp
        cjsruntime_object.cpp
        )