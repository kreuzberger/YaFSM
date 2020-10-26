install( TARGETS yafsmgen DESTINATION "yafsm")
install( DIRECTORY ${YaFSM_SOURCE_DIR}/src/cpp/inc ${YaFSM_SOURCE_DIR}/src/cpp/qt DESTINATION "yafsm")
install( DIRECTORY ${YaFSM_SOURCE_DIR}/src/cpp/stl ${YaFSM_SOURCE_DIR}/src/cpp/stl DESTINATION "yafsm")
install( FILES FindYaFSM.cmake DESTINATION "yafsm/cmake")

