cmake_minimum_required(VERSION 3.0)

project(phytest)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")
add_executable(phytest
  test.cpp
  )
