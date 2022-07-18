cmake_minimum_required(VERSION 3.8)
project(runexe)

set(CMAKE_CXX_STANDARD 98)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(SOURCE_FILES 
    src/configuration.cpp 
    src/invocation_params.cpp 
    src/invocation_result.cpp 
    src/invocation_verdict.cpp 
    src/strings.cpp 
    src/run.cpp 
    src/process.cpp 
    src/interaction.cpp)

add_library(limiter STATIC ${SOURCE_FILES} src/limiter.cpp)

add_executable(runexe ${SOURCE_FILES} src/entrypoint.cpp)
