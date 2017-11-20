cmake_minimum_required(VERSION 3.0)
project(gbtest)
include(../gbCMake/gbCommon.cmake)

<<<<<<< HEAD
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../gbCMake)
find_package(gbPhysics REQUIRED)

add_executable(gbtest
  test.cpp
  )

target_link_libraries(gbtest
  debug ${gbPhysics_LIBRARIES_d}
  optimized ${gbPhysics_LIBRARIES}
  )
=======
add_executable(gbtest
  ../src/gbAlgorithm.cpp
  test.cpp
  )
>>>>>>> 05ce7fb0ecc5c55b3403f9595b763e5221ed0333
