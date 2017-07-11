# - Find YaFsmGen
#
#  USE_YAFSM               - have the yafsmgen command found

include(CMakeParseArguments)


if( NOT "${YAFSM_SCRIPT}" STREQUAL "" AND NOT "${YAFSM_SCRIPT}" STREQUAL "YAFSM_SCRIPT-NOTFOUND")
  set( YAFSM_FOUND true )
else()
  find_program(XMLLINT_EXECUTABLE xmllint )
  if( UNIX )
    find_file(YAFSM_SCRIPT
              NAMES YaFsm.pl
              PATHS  $ENV{PATH} /usr/local/yafsm
              DOC "Perl script YaFsm.pl" )
  else()
    set(ENVPROGRAMFILES32 "PROGRAMFILES(X86)")
    find_file(YAFSM_SCRIPT
              NAMES YaFsm.pl
              PATHS  $ENV{PATH} "$ENV{PROGRAMFILES}/YaFsm/yafsm" "$ENV{${ENVPROGRAMFILES32}}/YaFsm/yafsm"
              DOC "Perl script YaFsm.pl" )
  endif()

  find_program(PERL_EXECUTABLE
           NAMES perl perl.exe )

  if( YAFSM_SCRIPT STREQUAL "YAFSM_SCRIPT-NOTFOUND" )
    set( YAFSM_INCLUDE_DIR )
    set( YAFSM_FOUND false )
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



set( YAFSM_USE_FILE ${CMAKE_CURRENT_LIST_DIR}/FindYaFSM.cmake)

if( YAFSM_FOUND )
 get_filename_component( YAFSM_INCLUDE_DIR ${YAFSM_SCRIPT} DIRECTORY )
 set( YAFSM_INCLUDE_DIRS ${YAFSM_INCLUDE_DIR} )
endif()

macro (YAFSM_GENERATE outfiles fsmFile)

  set(options)
  set(oneValueArgs OUTPUT_DIRECTORY )
  set(multiValueArgs  )

  cmake_parse_arguments(YAFSM_OPTIONS  "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
  if( "${YAFSM_OPTIONS_OUTPUT_DIRECTORY}" STREQUAL "" )
    set( YAFSM_OPTIONS_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" )
  endif()

  get_filename_component( fsmFileAbsolute ${fsmFile} ABSOLUTE )
  get_filename_component( fsm ${fsmFile} NAME_WE )
  set(outfile ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/I${fsm}.h
             ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/I${fsm}State.h
             ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/${fsm}.h
             ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/${fsm}StateBase.h
             ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/${fsm}StateImpl.h
             ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/${fsm}StateImpl.cpp
  )

  set(fileCode FSMTimer.h
               FSMTimer.cpp
               FSMEvent.h
               FSMEvent.cpp
               ScxmlFSMEvent.h
               ScxmlFSMEvent.cpp
  )
  set(fileIfc IFSMTimer.h
              IFSMTimerCB.h
              IFSMEvent.h
              IFSMEventCB.h
              IScxmlFSMEvent.h
              IScxmlFSMEventCB.h
  )

  add_custom_command( OUTPUT ${outfile}
    COMMAND ${CMAKE_COMMAND} -E echo ${PERL_EXECUTABLE} -I${YAFSM_INCLUDE_DIR} -f ${YAFSM_SCRIPT} --fsm=${fsmFileAbsolute}  --genview --gencode --outview=${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/view --outcode=${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code
    COMMAND ${PERL_EXECUTABLE} -I${YAFSM_INCLUDE_DIR} -f ${YAFSM_SCRIPT} --fsm=${fsmFileAbsolute}  --genview --gencode --outview=${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/view --outcode=${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code
    #ARGS -o ${outfile} ${fsmFile}
    DEPENDS ${fsmFileAbsolute} ${YAFSM_INCLUDE_DIRS}
  )
  #COMMENT "Generating FSM ${FSM_SRC}"
  foreach ( file ${fileCode} )
    add_custom_command(
    OUTPUT ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/${file}
    COMMAND ${CMAKE_COMMAND} -E copy ${YAFSM_INCLUDE_DIRS}/codeimpl/cpp/qt4/${file}  ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/
    )
    set( ${outfiles} ${${outfiles}} ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/${file})
  endforeach( file )

  foreach ( fileI ${fileIfc} )
    add_custom_command(
      OUTPUT ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/${fileI}
      COMMAND ${CMAKE_COMMAND} -E copy ${YAFSM_INCLUDE_DIRS}/codeimpl/cpp/inc/${fileI}  ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code
      DEPENDS ${YAFSM_INCLUDE_DIRS}/codeimpl/cpp/inc/${fileI}
    )
    set( ${outfiles} ${${outfiles}} ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/${fileI})
  endforeach( fileI )

  set( ${outfiles} ${${outfiles}} ${outfile})
  INCLUDE_DIRECTORIES( ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code )
  QT4_WRAP_CPP( GENERATED_FSM_SRC_MOC_HEADERS ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/FSMTimer.h ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/FSMEvent.h ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/ScxmlFSMEvent.h)
endmacro( YAFSM_GENERATE )

#mark_as_advanced( YAFSM_INCLUDE_DIRS )

