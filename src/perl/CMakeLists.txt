project(YaFSM_GENERATOR)

macro (YAFSM_GENERATE outfiles)
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
      COMMAND perl -I${YaFSM_GENERATOR_SOURCE_DIR} -f ${YaFSM_GENERATOR_SOURCE_DIR}/YaFsm.pl --fsm=${it}  --genview --gencode --outview=${CMAKE_CURRENT_BINARY_DIR}/${fsm}/view --outcode=${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code
      DEPENDS ${it} ${YaFSM_GENERATOR_SOURCE_DIR} )

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
endmacro( YAFSM_GENERATE )



install( FILES YaFsm.pl YaFsm.pm YaFsmCodeGen.pm YaFsmCodeGenCppOO.pm YaFsmParser.pm YaFsmViewerGen.pm YaFsmScxmlParser.pm YaFsmScxmlViewerGen.pm YaFsmScxmlCodeGenCppOO.pm "${CMAKE_SOURCE_DIR}/src/xml/yafsm.xsd" DESTINATION "yafsm")
if( UNIX )
  install( PROGRAMS yafsmgen DESTINATION "yafsm")
else()
  install( PROGRAMS yafsmgen.cmd DESTINATION "yafsm")
endif()

install( DIRECTORY codeimpl DESTINATION "yafsm")
install( DIRECTORY images DESTINATION "yafsm")



