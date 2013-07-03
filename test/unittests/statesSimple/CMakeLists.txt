project ( testSimpleFSM)
set( DESIRED_QT_VERSION 4 )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
include(${QT_USE_FILE})

FSM_GENERATE(GENERATED_FSM_SRC SimpleFSM.xml)
QT4_WRAP_CPP( GENERATED_TEST_MOC_HEADERS TestSimpleFSM.h)


add_executable(testsimplefsm TestSimpleFSM.cpp ${GENERATED_FSM_SRC} ${GENERATED_FSM_SRC_MOC_HEADERS} ${GENERATED_TEST_MOC_HEADERS})

target_link_libraries(testsimplefsm  ${QT_LIBRARIES})
add_test( TestSimpleFSM testsimplefsm)


