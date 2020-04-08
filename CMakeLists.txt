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
        cjsgen_ast.cpp
        cjsruntime.cpp
        cjsruntime.h
        cjsrt_array.cpp
        cjsrt_boolean.cpp
        cjsrt_function.cpp
        cjsrt_null.cpp
        cjsrt_number.cpp
        cjsrt_object.cpp
        cjsrt_regex.cpp
        cjsrt_string.cpp
        cjsrt_undefined.cpp
        )