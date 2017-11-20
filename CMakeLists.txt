cmake_minimum_required(VERSION 3.0)
project(gbPhysics)

include(gbCMake/gbCommon.cmake)

set(gb_phy_srcs)
gb_add_class(gbAlgorithm src gb_phy_srcs)
gb_add_class(gbMath src gb_phy_srcs)

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

install(TARGETS gbPhysics ARCHIVE DESTINATION lib)

set(headers)

foreach(src IN LISTS gb_phy_srcs)
  if(src MATCHES ".*\\.h")
    set(headers ${headers} ${src})
  endif()
endforeach()

install(FILES ${headers} DESTINATION include)
