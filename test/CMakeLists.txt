cmake_minimum_required(VERSION 3.0)
project(gbtest)
include(../gbCMake/gbCommon.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../gbCMake)

add_executable(gbtest
  test.cpp
  )

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ ${CMAKE_CURRENT_SOURCE_DIR}/../build)

target_link_libraries(gbtest
  gbPhysics
  )

set_target_properties(gbtest
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_BINARY_DIR}")
