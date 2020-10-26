project( xmlparsercpptest )

include_directories(  ${YaFSM_SOURCE_DIR}/src )
set( SRCS
  ${CMAKE_CURRENT_LIST_DIR}/../tinyxml/tinyxml2.cpp
  ${CMAKE_CURRENT_LIST_DIR}/main.cpp
  ${CMAKE_CURRENT_LIST_DIR}/YaFsmCppTest.cpp
  ${CMAKE_CURRENT_LIST_DIR}/../YaFsmScxmlParser.cpp
  ${CMAKE_CURRENT_LIST_DIR}/../YaFsm.cpp
)

set( HDRS
  ${CMAKE_CURRENT_LIST_DIR}/../tinyxml/tinyxml2.h
  ${CMAKE_CURRENT_LIST_DIR}/../YaFsmScxmlParser.h
  ${CMAKE_CURRENT_LIST_DIR}/YaFsmCppTest.h
  ${CMAKE_CURRENT_LIST_DIR}/../YaFsm.h
)

add_executable( yafsmgentest ${SRCS} ${HDRS} )


add_test( yafsmgentest yafsmgentest )
