project( xmlparsercpp )

set( SRCS
  ${CMAKE_CURRENT_LIST_DIR}/tinyxml/tinyxml2.cpp
  ${CMAKE_CURRENT_LIST_DIR}/main.cpp
  ${CMAKE_CURRENT_LIST_DIR}/YaFsmScxmlParser.cpp
  ${CMAKE_CURRENT_LIST_DIR}/YaFsm.cpp
)

set( HDRS
  ${CMAKE_CURRENT_LIST_DIR}/tinyxml/tinyxml2.h
  ${CMAKE_CURRENT_LIST_DIR}/YaFsmScxmlParser.h
  ${CMAKE_CURRENT_LIST_DIR}/YaFsm.h
)

add_executable( yafsmgen ${SRCS} ${HDRS} )

add_subdirectory(unittests)
