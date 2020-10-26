project ( testSimpleScxmlFSM)

YAFSM_GENERATE_CPP(GENERATED_FSM_SRC SimpleScxmlFSM.scxml)
add_executable(testsimplescxmlfsm TestSimpleScxmlFSM.cpp ${GENERATED_FSM_SRC})
#target_link_libraries( testsimplescxmlfsm  Qt5::Core Qt5::Test )
target_link_libraries( testsimplescxmlfsm Threads::Threads )
add_test( TestSimpleScxmlFSM testsimplescxmlfsm )


