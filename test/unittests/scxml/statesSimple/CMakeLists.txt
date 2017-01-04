project ( testSimpleScxmlFSM)
set( DESIRED_QT_VERSION 4 )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
include(${QT_USE_FILE})

FSM_GENERATE(GENERATED_FSM_SRC SimpleScxmlFSM.scxml)
QT4_WRAP_CPP( GENERATED_TEST_MOC_HEADERS TestSimpleScxmlFSM.h)


#add_executable(testsimplescxmlfsm TestSimpleScxmlFSM.cpp ${GENERATED_FSM_SRC} ${GENERATED_FSM_SRC_MOC_HEADERS} ${GENERATED_TEST_MOC_HEADERS})
#target_link_libraries( testsimplescxmlfsm  ${QT_LIBRARIES})
#add_test( TestSimpleScxmlFSM testsimplescxmlfsm )


