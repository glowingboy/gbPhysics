cmake_minimum_required(VERSION 3.0)
project(gbtest)
include(../gbCMake/gbCommon.cmake)

add_executable(gbtest
  ../src/gbAlgorithm.cpp
  test.cpp
  )
