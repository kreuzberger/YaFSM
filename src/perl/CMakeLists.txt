#add_subdirectory(images)
project(YaFSM_GENERATOR)

#ADD_CUSTOM_COMMAND(
#  OUTPUT FSM
#  COMMAND echo bla > ${CMAKE_CURRENT_SOURCE_DIR}/test.h
#CMAKE_CURRENT_BINARY_DIR
#)

#set( FSM_SRC test.xml)

#ADD_CUSTOM_TARGET(Generate ALL DEPENDS FSM)
#if ( FSM_SRC )
macro (FSM_GENERATE outfiles)
  foreach( it ${ARGN})
   get_filename_component( it ${it} ABSOLUTE )
   get_filename_component( fsm ${it} NAME_WE )
#   set(outfile ${CMAKE_CURRENT_BINARY_DIR}/code/I${outfile}.h ${CMAKE_CURRENT_BINARY_DIR}/code/${outfile}.h ${CMAKE_CURRENT_BINARY_DIR}/code/${outfile}StateBase.h ${CMAKE_CURRENT_BINARY_DIR}/code/I${outfile}.h${CMAKE_CURRENT_BINARY_DIR}/code/I${outfile}State.h)
   set(outfile ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/I${fsm}.h
               ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/I${fsm}State.h
               ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${fsm}.h
               ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${fsm}StateBase.h
               ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${fsm}StateImpl.h
               ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${fsm}StateImpl.cpp
   )

   set(fileCode FSMTimer.h
                FSMTimer.cpp
                FSMEvent.h
                FSMEvent.cpp
   )
   set(fileIfc  IFSMTimer.h
                IFSMTimerCB.h
                IFSMEvent.h
                IFSMEventCB.h
   )

  add_custom_command( OUTPUT ${outfile}
      #COMMAND perl -I${YaFSM_GENERATOR_SOURCE_DIR} -f ${YaFSM_GENERATOR_SOURCE_DIR}/YaFsm.pl --fsm=${it}  --gendot --gendsc --gencode --verbose --outdot=${CMAKE_CURRENT_BINARY_DIR}/${fsm}/dot --outdsc=${CMAKE_CURRENT_BINARY_DIR}/${fsm}/dsc --outcode=${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code
      COMMAND perl -I${YaFSM_GENERATOR_SOURCE_DIR} -f ${YaFSM_GENERATOR_SOURCE_DIR}/YaFsm.pl --fsm=${it}  --genview --gencode --outview=${CMAKE_CURRENT_BINARY_DIR}/${fsm}/view --outcode=${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code
      #ARGS -o ${outfile} ${it}
      DEPENDS ${it} ${YaFSM_GENERATOR_SOURCE_DIR} )
      #COMMENT "Generating FSM ${FSM_SRC}"
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
  QT4_WRAP_CPP( GENERATED_FSM_SRC_MOC_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/FSMTimer.h ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/FSMEvent.h)
 endforeach( it )
endmacro( FSM_GENERATE )
#endif( FSM_SRC )




install( PROGRAMS yafsmgen DESTINATION "bin")
install( FILES YaFsm.pl YaFsm.pm YaFsmCodeGen.pm YaFsmCodeGenCppOO.pm YaFsmParser.pm YaFsmViewerGen.pm "${CMAKE_SOURCE_DIR}/src/xml/yafsm.xsd" DESTINATION "yafsm")
install( DIRECTORY codeimpl DESTINATION "yafsm")
install( DIRECTORY images DESTINATION "yafsm")



