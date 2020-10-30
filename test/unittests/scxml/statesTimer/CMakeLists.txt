project ( testTimerScxmlFSM)

YAFSM_GENERATE_CPP(GENERATED_FSM_SRC TimerScxmlFSM.scxml)

add_executable(testtimerscxmlfsm TestTimerScxmlFSM.cpp ${GENERATED_FSM_SRC})
target_link_libraries( testtimerscxmlfsm Qt5::Core Qt5::Test )
add_test( TestTimerScxmlFSM testtimerscxmlfsm )


