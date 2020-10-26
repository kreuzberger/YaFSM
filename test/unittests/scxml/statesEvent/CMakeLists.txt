project ( testEventScxmlFSM)
YAFSM_GENERATE_CPP(GENERATED_FSM_SRC EventScxmlFSM.scxml)

add_executable(testeventscxmlfsm TestEventScxmlFSM.cpp ${GENERATED_FSM_SRC})
target_link_libraries( testeventscxmlfsm Threads::Threads )
add_test( TestEventScxmlFSM testeventscxmlfsm )


