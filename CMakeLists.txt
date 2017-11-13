cmake_minimum_required(VERSION 3.0)

project(gbPhysics)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/CMake/)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-DGB_DEBUG=1")
endif()

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")

set(gb_phy_srcs
  gbAlgorithm.cpp
  )

add_library(gbPhysics STATIC
  ${gb_phy_srcs}
  gbAlgorithm.h
  )

set_target_properties(gbPhysics
  PROPERTIES
  DEBUG_OUTPUT_NAME gbPhysicsd
  RELEASE_OUTPUT_NAME gbPhysics
  )

# install
if(UNIX)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/gbPhysics)
endif()

message("install dir: " ${CMAKE_INSTALL_PREFIX})

install(TARGETS gbPhysics ARCHIVE DESTINATION lib)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include
  FILES_MATCHING PATTERN "*.h"
  )
