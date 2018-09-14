cmake_minimum_required(VERSION 3.0)
project(gbPhysics)

include(gbCMake/gbCommon.cmake)

set(GB_PHYSICS_VERSION_MAJOR 0)
set(GB_PHYSICS_VERSION_MINOR 0)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h)

set(srcs)
gb_add_class(physicsNS src srcs)
gb_add_class(type src srcs)
gb_add_class(matrix src srcs)
gb_add_class(math src srcs)
gb_add_class(image src srcs)
gb_add_class(boundingbox src srcs)
gb_add_class(sptree src srcs)
gb_add_class(camera src srcs)
gb_add_class(ray src srcs)
gb_add_class(plane src srcs)

add_library(gbPhysics STATIC
  ${srcs}
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

foreach(src IN LISTS srcs)
  if(src MATCHES ".*\\.h")
    set(headers ${headers} ${src})
  endif()
endforeach()

set(headers ${headers} ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h)

install(FILES ${headers} DESTINATION .)
# install Eigen lib
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/Eigen DESTINATION .)
