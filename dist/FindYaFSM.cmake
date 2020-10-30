# - Find YaFsmGen
#
#  USE_YAFSM               - have the yafsmgen command found

include(CMakeParseArguments)

if( NOT YaFSM_FIND_COMPONENTS)
    set( YaFSM_FIND_COMPONENTS Generator)
    set( YaFSM_FIND_REQUIRED_Generator TRUE )
endif()


unset (_YaFSM_REQUIRED_VARS)

if( "Generator" IN_LIST YaFSM_FIND_COMPONENTS)
    if( YaFSM_FIND_REQUIRED_Generator)
        list( APPEND _YaFSM_REQUIRED_VARS YAFSMGEN)
    endif()

    find_program(YAFSMGEN
          NAMES yafsmgen
          PATHS ${CMAKE_CURRENT_LIST_DIR}/../bin
          DOC "yafsm executable"
          NO_DEFAULT_PATH
    )


    if( NOT YAFSMGEN STREQUAL "YAFSMGEN-NOTFOUND" )
        set( YaFSM_Generator_FOUND TRUE )
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args (YaFSM
  REQUIRED_VARS
    ${_YaFSM_REQUIRED_VARS}
  HANDLE_COMPONENTS
)

if(YaFSM_Generator_FOUND AND NOT TARGET YaFSM::Generator)
  add_executable(YaFSM::Generator IMPORTED GLOBAL)
  set_property(TARGET YaFSM::Generator
               PROPERTY IMPORTED_LOCATION "${YAFSMGEN}")
endif()

include( ${CMAKE_CURRENT_LIST_DIR}/YaFSM.cmake)
