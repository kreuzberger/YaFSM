# - Find YaFsmGen
#
#  USE_YAFSM               - have the yafsmgen command found

include(CMakeParseArguments)


if( NOT "${YAFSMGEN}" STREQUAL "" AND NOT "${YAFSMGEN}" STREQUAL "YAFSMGEN-NOTFOUND")
  set( YAFSM_FOUND true )
else()
  if( UNIX )
    find_program(YAFSMGEN
              NAMES yafsmgen
              PATHS  $ENV{PATH} /usr/local/yafsm
              DOC "yafsm executable" )
  else()
    set(ENVPROGRAMFILES32 "PROGRAMFILES")
    find_program(YAFSMGEN
              NAMES yafsmgen.exe
              PATHS  $ENV{PATH} "$ENV{PROGRAMFILES}/YaFsm/yafsm"
              DOC "Perl script YaFsm.pl" )
  endif()


  if( YAFSMGEN STREQUAL "YAFSMGEN-NOTFOUND" )
    set( YAFSM_INCLUDE_DIR )
    set( YAFSM_FOUND false )
  else()
      if(NOT YaFSM_FIND_QUIETLY)
        message(STATUS "Found YaFSM! " ${YAFSMGEN})
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
 get_filename_component( YAFSM_INCLUDE_DIR ${YAFSMGEN} DIRECTORY )
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
  set(outfile ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/I${fsm}State.h
             ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/${fsm}.h
             ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/${fsm}StateBase.h
             ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/${fsm}StateImpl.h
             ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/${fsm}StateImpl.cpp
  )

  set(fileCode ScxmlFSMEvent.h
               ScxmlFSMEvent.cpp
  )
  set(fileIfc IScxmlFSMEvent.h
              IScxmlFSMEventCB.h
  )

  add_custom_command( OUTPUT ${outfile}
    COMMAND ${CMAKE_COMMAND} -E echo ${YAFSMGEN} --fsm ${fsmFileAbsolute} --outcode ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code
    COMMAND ${YAFSMGEN} --fsm ${fsmFileAbsolute} --outcode ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code
    DEPENDS ${fsmFileAbsolute} ${YAFSM_INCLUDE_DIRS} ${YAFSMGEN}
  )
  foreach ( file ${fileCode} )
    add_custom_command(
    OUTPUT ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/${file}
    COMMAND ${CMAKE_COMMAND} -E copy ${YAFSM_INCLUDE_DIRS}/qt4/${file}  ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/
    )
    set( ${outfiles} ${${outfiles}} ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/${file})
  endforeach( file )

  foreach ( fileI ${fileIfc} )
    add_custom_command(
      OUTPUT ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/${fileI}
      COMMAND ${CMAKE_COMMAND} -E copy ${YAFSM_INCLUDE_DIRS}/inc/${fileI}  ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code
      DEPENDS ${YAFSM_INCLUDE_DIRS}/inc/${fileI}
    )
    set( ${outfiles} ${${outfiles}} ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/${fileI})
  endforeach( fileI )

  set( ${outfiles} ${${outfiles}} ${outfile})
  INCLUDE_DIRECTORIES( ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code )
  QT4_WRAP_CPP( GENERATED_FSM_SRC_MOC_HEADERS ${YAFSM_OPTIONS_OUTPUT_DIRECTORY}/${fsm}/code/ScxmlFSMEvent.h)
endmacro( YAFSM_GENERATE )


