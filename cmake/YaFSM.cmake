#[=======================================================================[.rst:

YaFSM
*****

Script to add Yet Another FSM scxml definitions to the build process.

.. command:: yafsm_generate_cpp

  Macro to generate source code files from scxml DEFINITIONS

  ::

    yafsm_generate_cpp( outfiles
                        FSM scxml
                        [NAMESPACE namespace]
    )

  The options are:

    ``outfiles``
      The generated cpp files that could be added to a target.

    ``FSM scxml``
      Full path to the SCXML file describing the FSM.

    ``NAMESPACE namespace``
      The generated source files could be generated in the given namespace
      to avoid redefinition problems of the FSM triggers.
      The namespace must not, but should be provided.

#]=======================================================================]


include(CMakeParseArguments)

macro (YAFSM_GENERATE_CPP outfiles)

  set(options)
  set(oneValueArgs NAMESPACE FSM )
  set(multiValueArgs  )

  cmake_parse_arguments( YAFSM_OPTIONS "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  if( YAFSM_OPTIONS_NAMESPACE )
    set( YAFSM_OPT --namespace ${YAFSM_OPTIONS_NAMESPACE} )
  endif()

  if(NOT "${YAFSM_OPTIONS_UNPARSED_ARGUMENTS}" STREQUAL "")
    # ARGN is not a variable: assign its value to a variable
     set(YaFSMExtraMacroArgs ${ARGN})

     # Get the length of the list
   list(LENGTH YaFSMExtraMacroArgs NumExtraMacroArgs)

   # Execute the following block only if the length is > 0
   if(NumExtraMacroArgs GREATER 0)
     set( YAFSM_OPTIONS_FSM ${ARGV1} )
     list( REMOVE_ITEM YaFSMExtraMacroArgs ${YAFSM_OPTIONS_FSM} )
     foreach(ExtraArg ${YaFSMExtraMacroArgs})
       message(WARNING ">>> Skipping argument ${ExtraArg}")
     endforeach()
   endif()

  endif()


  get_filename_component( it ${YAFSM_OPTIONS_FSM} ABSOLUTE )
  get_filename_component( fsm ${YAFSM_OPTIONS_FSM} NAME_WE )
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
    COMMAND ${CMAKE_COMMAND} -E  echo $<TARGET_FILE:YaFSM::Generator> --fsm ${it}  ${YAFSM_OPT} --outcode ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code --verbose
    COMMAND $<TARGET_FILE:YaFSM::Generator> --fsm ${it}  ${YAFSM_OPT} --outcode ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code
    DEPENDS ${it} YaFSM::Generator )
  set( ${outfiles} ${${outfiles}} ${outfile})

  get_target_property(YaFSM_CODE_SOURCE_DIR YaFSM::Code INTERFACE_SOURCES)

  foreach ( file ${fileCode} )
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${file}
      COMMAND ${CMAKE_COMMAND} -E echo copy  ${YaFSM_CODE_SOURCE_DIR}/${file}  ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code
      COMMAND ${CMAKE_COMMAND} -E copy  ${YaFSM_CODE_SOURCE_DIR}/${file}  ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code
      DEPENDS  ${YaFSM_CODE_SOURCE_DIR}/${file}
    )
    set( ${outfiles} ${${outfiles}} ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${file})
  endforeach( file )

  get_target_property(YaFSM_CODE_INCLUDE_DIR YaFSM::Code INTERFACE_INCLUDE_DIRECTORIES)

  foreach ( fileI ${fileIfc} )
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${fileI}
      COMMAND ${CMAKE_COMMAND} -E copy ${YaFSM_CODE_INCLUDE_DIR}/${fileI}  ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code
      DEPENDS ${YaFSM_CODE_INCLUDE_DIR}/${fileI}
    )
    set( ${outfiles} ${${outfiles}} ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/${fileI})
  endforeach( fileI )

  INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code )
  QT5_WRAP_CPP( GENERATED_FSM_SRC_MOC_HEADERS  ${CMAKE_CURRENT_BINARY_DIR}/${fsm}/code/ScxmlFSMEvent.h)
  set( ${outfiles} ${${outfiles}} ${GENERATED_FSM_SRC_MOC_HEADERS})
endmacro( YAFSM_GENERATE_CPP )
