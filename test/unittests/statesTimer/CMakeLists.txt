project ( testTimerFSM)
set( DESIRED_QT_VERSION 4 )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
include(${QT_USE_FILE})
#set ( FSM_SRC fullFSM.xml )
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
#add_executable(testfullfsm ${FSM})
FSM_GENERATE(GENERATED_FSM_SRC TimerFSM.xml)
QT4_WRAP_CPP( GENERATED_TEST_MOC_HEADERS TestTimerFSM.h)

#INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
#QT4_WRAP_CPP( GENERATED_FSM_SRC_MOC_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/FullFSM/code/FSMTimer.h)
add_executable(testtimerfsm TestTimerFSM.cpp ${GENERATED_FSM_SRC} ${GENERATED_FSM_SRC_MOC_HEADERS} ${GENERATED_TEST_MOC_HEADERS})
#add_custom_target( GenerateFSM ALL
#  DEPENDS ${FSM_SRC}
#  )
target_link_libraries(testtimerfsm ${QT_LIBRARIES})
add_test( TestTimerFSM testtimerfsm)


#fullFSM.xml

