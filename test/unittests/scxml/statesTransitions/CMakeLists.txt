project ( testTransScxmlFSM)
set( DESIRED_QT_VERSION 4 )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
include(${QT_USE_FILE})

YAFSM_GENERATE_CPP(GENERATED_FSM_SRC TransScxmlFSM.scxml)
QT4_WRAP_CPP( GENERATED_TEST_MOC_HEADERS TestTransScxmlFSM.h TransScxmlDataModel.h)


add_executable(testtransscxmlfsm TestTransScxmlFSM.cpp ${GENERATED_FSM_SRC} ${GENERATED_FSM_SRC_MOC_HEADERS} ${GENERATED_TEST_MOC_HEADERS})
target_link_libraries( testtransscxmlfsm  ${QT_LIBRARIES})
add_test( TestTransScxmlFSM testtransscxmlfsm )


