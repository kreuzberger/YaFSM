project ( testActionFSM)
set( DESIRED_QT_VERSION 4 )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
include(${QT_USE_FILE})
#set ( FSM_SRC fullFSM.xml )
#INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
#add_executable(testfullfsm ${FSM})
FSM_GENERATE(GENERATED_FSM_SRC ActionFSM.xml)
#INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
#include_directories(/usr/include/unittest++)
QT4_WRAP_CPP( GENERATED_TEST_MOC_HEADERS TestActionFSM.h)
add_executable(testactionfsm TestActionFSM.cpp ${GENERATED_FSM_SRC} ${GENERATED_FSM_SRC_MOC_HEADERS} ${GENERATED_TEST_MOC_HEADERS})
#add_custom_target( GenerateFSM ALL
#  DEPENDS ${FSM_SRC}
#  )
target_link_libraries(testactionfsm ${QT_LIBRARIES})
add_test( TestActionFSM testactionfsm)


#fullFSM.xml

