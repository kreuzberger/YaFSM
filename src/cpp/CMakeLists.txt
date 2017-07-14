project( xmlparsercpp )


macro (YAFSM_GENERATE_CPP outfiles)
  foreach( it ${ARGN})
    get_filename_component( it ${it} ABSOLUTE )
    get_filename_component( fsm ${it} NAME_WE )
    set(outfile ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/I${fsm}State.h
               ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${fsm}.h
               ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${fsm}StateBase.h
               ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${fsm}StateImpl.h
               ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${fsm}StateImpl.cpp
    )

    set(fileCode ScxmlFSMEvent.h
                 ScxmlFSMEvent.cpp
    )
    set(fileIfc IScxmlFSMEvent.h
                IScxmlFSMEventCB.h
    )

    add_custom_command( OUTPUT ${outfile}
      COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code"
      COMMAND $<TARGET_FILE:yafsmgen> --fsm ${it}  --outcode ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code --verbose
      DEPENDS ${it} ${YaFSM_GENERATOR_SOURCE_DIR} yafsmgen )

    foreach ( file ${fileCode} )
      add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${file}
        COMMAND ${CMAKE_COMMAND} -E copy ${YaFSM_GENERATOR_SOURCE_DIR}/codeimpl/cpp/qt4/${file}  ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code
        DEPENDS ${YaFSM_GENERATOR_SOURCE_DIR}/codeimpl/cpp/qt4/${file}
      )
      set( ${outfiles} ${${outfiles}} ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${file})
    endforeach( file )

    foreach ( fileI ${fileIfc} )
      add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${fileI}
        COMMAND ${CMAKE_COMMAND} -E copy ${YaFSM_GENERATOR_SOURCE_DIR}/codeimpl/cpp/inc/${fileI}  ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code
        DEPENDS ${YaFSM_GENERATOR_SOURCE_DIR}/codeimpl/cpp/inc/${fileI}
      )
      set( ${outfiles} ${${outfiles}} ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${fileI})
    endforeach( fileI )

    set( ${outfiles} ${${outfiles}} ${outfile})
    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code )
    QT4_WRAP_CPP( GENERATED_FSM_SRC_MOC_HEADERS  ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/ScxmlFSMEvent.h)
  endforeach( it )
endmacro( YAFSM_GENERATE_CPP )



set( SRCS
  ${CMAKE_CURRENT_LIST_DIR}/tinyxml/tinyxml2.cpp
  ${CMAKE_CURRENT_LIST_DIR}/main.cpp
  ${CMAKE_CURRENT_LIST_DIR}/YaFsmScxmlParser.cpp
  ${CMAKE_CURRENT_LIST_DIR}/YaFsm.cpp
)

set( HDRS
  ${CMAKE_CURRENT_LIST_DIR}/tinyxml/tinyxml2.h
  ${CMAKE_CURRENT_LIST_DIR}/YaFsmScxmlParser.h
  ${CMAKE_CURRENT_LIST_DIR}/YaFsm.h
)

add_executable( yafsmgen ${SRCS} ${HDRS} )

add_subdirectory(unittests)
