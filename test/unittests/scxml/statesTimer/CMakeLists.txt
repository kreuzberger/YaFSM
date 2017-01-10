project ( testTimerScxmlFSM)
set( DESIRED_QT_VERSION 4 )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
include(${QT_USE_FILE})

FSM_GENERATE(GENERATED_FSM_SRC TimerScxmlFSM.scxml)
QT4_WRAP_CPP( GENERATED_TEST_MOC_HEADERS TestTimerScxmlFSM.h TimerScxmlFSMDataModel.h)


add_executable(testtimerscxmlfsm TestTimerScxmlFSM.cpp TimerScxmlFSMDataModel.cpp ${GENERATED_FSM_SRC} ${GENERATED_FSM_SRC_MOC_HEADERS} ${GENERATED_TEST_MOC_HEADERS})
target_link_libraries( testtimerscxmlfsm  ${QT_LIBRARIES})
add_test( TestTimerScxmlFSM testtimerscxmlfsm )


