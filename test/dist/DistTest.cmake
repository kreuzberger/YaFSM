execute_process( COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_INSTALL_PREFIX}/dist-test-build-Debug )
execute_process( COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_INSTALL_PREFIX}/dist-test-build-Release )

foreach( buildtype "Release" "Debug")

    execute_process( COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/dist-test-build-${buildtype} )
    execute_process( COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=${buildtype} -DQt5_DIR=${Qt5_DIR} ${CMAKE_INSTALL_PREFIX}
                   RESULT_VARIABLE BUILD_STATUS
                   WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/dist-test-build-${buildtype}

    )

    if( NOT "${BUILD_STATUS}" STREQUAL "0" )
        message("dist cmake configure ${buildtype} returned: ${BUILD_STATUS}")
        message( FATAL_ERROR "error: dist cmake configure ${buildtype} failed!")
    endif()


    execute_process( COMMAND ${CMAKE_COMMAND} --build . --target all
                     RESULT_VARIABLE BUILD_STATUS
                     WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/dist-test-build-${buildtype}
    )

    if( NOT "${BUILD_STATUS}" STREQUAL "0" )
        message("dist build ${buildtype} returned: ${BUILD_STATUS}")
        message( FATAL_ERROR "error: dist build ${buildtype} failed!")
    endif()


    execute_process( COMMAND ctest --output-on-failure
                     RESULT_VARIABLE BUILD_STATUS
                     WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}/dist-test-build-${buildtype}
    )

    if( NOT "${BUILD_STATUS}" STREQUAL "0" )
        message("dist ctest ${buildtype} returned: ${BUILD_STATUS}")
        message( FATAL_ERROR "error: dist ctest ${buildtype} failed!")
    endif()

endforeach()
