project ( testTransScxmlFSM)

YAFSM_GENERATE_CPP(GENERATED_FSM_SRC TransScxmlFSM.scxml)

add_executable(testtransscxmlfsm TestTransScxmlFSM.cpp ${GENERATED_FSM_SRC})
target_link_libraries( testtransscxmlfsm Qt5::Core )
add_test( TestTransScxmlFSM testtransscxmlfsm )
