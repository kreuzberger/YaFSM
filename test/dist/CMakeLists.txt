if( UNIX )
  # test for dev package depends on install of runtime and develpment packages

  add_test( NAME dist-install
            COMMAND ${CMAKE_COMMAND} -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/disttest-install -DDISTTEST_TEST_DIR=${YaFSM_SOURCE_DIR}/test -DDISTTEST_TEMPLATE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/template -P ${CMAKE_CURRENT_SOURCE_DIR}/DistTestInstall.cmake
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
  set_tests_properties( dist-install PROPERTIES FIXTURES_SETUP disttest )

  add_test( NAME dist-cmake
            COMMAND ${CMAKE_COMMAND} -DQt5_DIR=${Qt5_DIR} -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/disttest-install -P ${CMAKE_CURRENT_SOURCE_DIR}/DistTest.cmake
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )

  set_tests_properties( "dist-cmake" PROPERTIES FIXTURES_REQUIRED disttest )
endif()

