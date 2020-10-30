project ( testEventScxmlFSM)
YAFSM_GENERATE_CPP(GENERATED_FSM_SRC EventScxmlFSM.scxml)

add_executable(testeventscxmlfsm TestEventScxmlFSM.cpp ${GENERATED_FSM_SRC})
target_link_libraries( testeventscxmlfsm Qt5::Core Qt5::Test )
add_test( TestEventScxmlFSM testeventscxmlfsm )
