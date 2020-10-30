cmake_minimum_required(VERSION 3.9)
project( YaFSM-Test)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   message(FATAL_ERROR "In-source builds are not allowed.")
endif()

add_subdirectory(yafsm)

# Force C++ standard, do not fall back, do not use compiler extensions
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()
include(CTest)

find_package( Qt5 COMPONENTS Core Test REQUIRED )

if( TARGET Qt5::Test )
  add_subdirectory(test)
else()
  message( WARNING "Disable automatic tests due to not configured Qt5::Core depenceny")
endif()
 
