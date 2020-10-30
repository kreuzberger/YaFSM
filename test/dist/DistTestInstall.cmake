execute_process( COMMAND ${CMAKE_COMMAND} -D "CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}" -P "${CMAKE_BINARY_DIR}/cmake_install.cmake" )
execute_process( COMMAND ${CMAKE_COMMAND} -E copy_directory ${DISTTEST_TEMPLATE_DIR}/ ${CMAKE_INSTALL_PREFIX} )
execute_process( COMMAND ${CMAKE_COMMAND} -E copy_directory ${DISTTEST_TEST_DIR}/unittests ${CMAKE_INSTALL_PREFIX}/test/unittests )
execute_process( COMMAND ${CMAKE_COMMAND} -E copy_directory ${DISTTEST_TEST_DIR}/include ${CMAKE_INSTALL_PREFIX}/test/include )
