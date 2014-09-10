# - Find YaFsmGen
#
#  USE_YAFSM               - have the yafsmgen command found



if( YAFSM_INCLUDE_DIRS )
  set( YAFSM_FOUND true )
else()
  find_program(XMLLINT_EXECUTABLE xmllint )
  find_file(YAFSM_SCRIPT
            NAMES YaFsm.pl
            PATHS  $ENV{PATH} /usr/local/yafsm )

  find_path(YAFSM_INCLUDE_DIR
    NAMES YaFsm.pl
    PATHS $ENV{PATH} /usr/local/yafsm )

  find_program(PERL_EXECUTABLE
           NAMES perl perl.exe )

  if( YAFSM_SCRIPT STREQUAL "YAFSM_SCRIPT-NOTFOUND" )
    set( USE_YAFSM false )
    set( YAFSM_INCLUDE_DIR )
  else()
    if( NOT PERL_EXECUTABLE STREQUAL "PERL_EXECUTABLE-NOTFOUND" )
      set( YAFSM_FOUND true )
      #message(STATUS "YaFSM.pl was found")

      if(NOT YaFSM_FIND_QUIETLY)
        message(STATUS "Found YaFSM! " ${YAFSM_SCRIPT})
      endif()
    endif()

    if( NOT YAFSM_FOUND )
      if(YaFSM_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find YaFSM")
      endif()
    endif()

  endif()
endif()

if(NOT "${YAFSM_SCRIPT}" STREQUAL "YAFSM_SCRIPT-NOTFOUND")
  set( YAFSM_COMMAND "${PERL_EXECUTABLE} -I${YAFSM_INCLUDE_DIR} -f ${YAFSM_SCRIPT}")
endif()


set( YAFSM_USE_FILE ${CMAKE_CURRENT_LIST_DIR}/FindYaFSM.cmake)
set( YAFSM_INCLUDE_DIRS ${YAFSM_INCLUDE_DIR} )

macro (YAFSM_GENERATE outfiles)
  foreach( it ${ARGN})
   get_filename_component( it ${it} ABSOLUTE )
   get_filename_component( fsm ${it} NAME_WE )
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
      COMMAND ${YAFSM_COMMAND} --fsm=${it}  --genview --gencode --outview=${CMAKE_CURRENT_BINARY_DIR}/${fsm}/view --outcode=${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code
      #ARGS -o ${outfile} ${it}
      DEPENDS ${it} ${YAFSM_INCLUDE_DIRS} )
      #COMMENT "Generating FSM ${FSM_SRC}"
    foreach ( file ${fileCode} )
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${file}
      COMMAND ${CMAKE_COMMAND} -E copy ${YAFSM_INCLUDE_DIRS}/codeimpl/cpp/qt4/${file}  ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code
      DEPENDS ${YAFSM_INCLUDE_DIRS}/codeimpl/cpp/qt4/${file}
    )
    set( ${outfiles} ${${outfiles}} ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${file})
    endforeach( file )

    foreach ( fileI ${fileIfc} )
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${fileI}
      COMMAND ${CMAKE_COMMAND} -E copy ${YAFSM_INCLUDE_DIRS}/codeimpl/cpp/inc/${fileI}  ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code
      DEPENDS ${YAFSM_INCLUDE_DIRS}/codeimpl/cpp/inc/${fileI}
    )
    set( ${outfiles} ${${outfiles}} ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${fileI})
    endforeach( fileI )

  set( ${outfiles} ${${outfiles}} ${outfile})
  INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code )
  QT4_WRAP_CPP( GENERATED_FSM_SRC_MOC_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/FSMTimer.h ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/FSMEvent.h)
 endforeach( it )
endmacro( YAFSM_GENERATE )

mark_as_advanced( YAFSM_INCLUDE_DIRS )

