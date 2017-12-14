cmake_minimum_required(VERSION 3.0)
project(gbPhysics)

include(gbCMake/gbCommon.cmake)

set(GB_PHYSICS_VERSION_MAJOR 0)
set(GB_PHYSICS_VERSION_MINOR 0)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h)

set(gb_phy_srcs)
gb_add_class(algorithm src gb_phy_srcs)
gb_add_class(math src gb_phy_srcs)
gb_add_class(image src gb_phy_srcs)

add_library(gbPhysics STATIC
  ${gb_phy_srcs}
  )

set_target_properties(gbPhysics
  PROPERTIES
  DEBUG_OUTPUT_NAME gbPhysicsd
  RELEASE_OUTPUT_NAME gbPhysics
  )

# install
set(CMAKE_INSTALL_PREFIX ${GB_DEFAULT_INSTALL_DIR}/gbPhysics)

install(TARGETS gbPhysics ARCHIVE DESTINATION .)

set(headers)

foreach(src IN LISTS gb_phy_srcs)
  if(src MATCHES ".*\\.h")
    set(headers ${headers} ${src})
  endif()
endforeach()

set(headers ${headers} ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h)

install(FILES ${headers} DESTINATION .)
