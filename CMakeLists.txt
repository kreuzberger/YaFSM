cmake_minimum_required(VERSION 3.9)
project( YaFSM)

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   message(FATAL_ERROR "In-source builds are not allowed.")
endif()

include ( cmake/YaFSM.cmake )

# Force C++ standard, do not fall back, do not use compiler extensions
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set up Qt stuff:
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC OFF)

set(CMAKE_INSTALL_PREFIX "" )
set( CMAKE_INSTALL_MESSAGE "LAZY" ) # one of NEVER, LAZY, ALWAYS

enable_testing()
include(CTest)



add_subdirectory(doc)
add_subdirectory(src)
add_executable(YaFSM::Generator ALIAS yafsmgen)
add_library( YaFSM::Code ALIAS Code )

#find_package ( Threads REQUIRED)

find_package( Qt5 COMPONENTS Core Test)

if( TARGET Qt5::Test )
  add_subdirectory(test)
else()
  message( WARNING "Disable automatic tests due to not configured Qt5::Core depenceny")
endif()

add_subdirectory(dist)
