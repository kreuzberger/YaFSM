project ( testEventScxmlFSM)
set( DESIRED_QT_VERSION 4 )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
include(${QT_USE_FILE})

FSM_GENERATE(GENERATED_FSM_SRC EventScxmlFSM.scxml)
QT4_WRAP_CPP( GENERATED_TEST_MOC_HEADERS TestEventScxmlFSM.h EventScxmlFSMDataModel.h)


add_executable(testeventscxmlfsm TestEventScxmlFSM.cpp EventScxmlFSMDataModel.cpp ${GENERATED_FSM_SRC} ${GENERATED_FSM_SRC_MOC_HEADERS} ${GENERATED_TEST_MOC_HEADERS})
target_link_libraries( testeventscxmlfsm  ${QT_LIBRARIES})
add_test( TestEventScxmlFSM testeventscxmlfsm )


