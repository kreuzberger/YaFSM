install( TARGETS yafsmgen DESTINATION "yafsm/bin")
install( DIRECTORY ${YaFSM_SOURCE_DIR}/src/cpp/inc ${YaFSM_SOURCE_DIR}/src/cpp/qt DESTINATION "yafsm/src")
install( FILES FindYaFSM.cmake ${YaFSM_SOURCE_DIR}/cmake/YaFSM.cmake DESTINATION "yafsm/cmake")
install( FILES CMakeLists.txt.in DESTINATION "yafsm" RENAME CMakeLists.txt)

