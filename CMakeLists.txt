cmake_minimum_required(VERSION 3.0)

project(gbPhysics)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/gbCMake/)

include(gbCMake/gbCommon.cmake)

set(gb_phy_srcs)
gb_add_class(gbAlgorithm . gb_phy_srcs)
gb_add_class(gbMath . gb_phy_srcs)

message("srcs: " ${gb_phy_srcs})

add_library(gbPhysics STATIC
  ${gb_phy_srcs}
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

file(GLOB headers . "*.h")
message("headers: " ${headers})
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ DESTINATION include
  FILES_MATCHING PATTERN "*.h"
  PATTERN "build" EXCLUDE
  PATTERN ".*" EXCLUDE
  PATTERN "gbCMake" EXCLUDE
  PATTERN "phytest" EXCLUDE
  )
