cmake_minimum_required(VERSION 3.8)
project(runexe)

set(CMAKE_CXX_STANDARD 98)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(SOURCE_FILES EntryPoint.cpp Configuration.cpp InvocationParams.cpp InvocationResult.cpp InvocationVerdict.cpp Strings.cpp Run.cpp Process.cpp Interaction.cpp)
add_executable(runexe ${SOURCE_FILES})
