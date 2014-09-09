project( yafsmviewer )
set( DESIRED_QT_VERSION 4 )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )

set (QT_USE_QTWEBKIT 1)
set (SOURCES
  main.cpp
  mainwindow.cpp
  yafsmviewer.cpp
  treeitem.cpp
  treemodel.cpp
)

QT4_ADD_RESOURCES( SAMPLE_RC_SRCS yafsmviewer.qrc )


#QT4_WRAP_CPP( YaFSMVIEWER_MOC_HEADERS treeitem.h treemodel.h mainwindow.h yafsmviewer.h)
QT4_WRAP_CPP( YaFSMVIEWER_MOC_HEADERS treemodel.h mainwindow.h yafsmviewer.h)
QT4_WRAP_UI( YaFSMVIEWER_UI_FILES yafsmviewer.ui )
include(${QT_USE_FILE})
#INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
add_executable(yafsmviewer ${SOURCES} ${YaFSMVIEWER_UI_FILES} ${SAMPLE_RC_SRCS} ${YaFSMVIEWER_MOC_HEADERS})
target_link_libraries(yafsmviewer ${QT_LIBRARIES} )


install(TARGETS yafsmviewer DESTINATION "yafsm"
        BUNDLE DESTINATION "yafsm"
)

#install(FILES ${QT_LIBRARIES} DESTINATION "yafsm")

# directories to look for dependencies
if(WIN32)
  set( APPS "${CMAKE_INSTALL_PREFIX}/yafsm/yafsmviewer.exe")
else()
  set( APPS "${CMAKE_INSTALL_PREFIX}/yafsm/yafsmviewer")
endif()
set( DIRS "${QT_LIBRARY_DIR}")
set( LIBS "" )

    #file(GLOB_RECURSE QTPLUGINS
    #  \"\${CMAKE_INSTALL_PREFIX}/${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")

INSTALL(CODE "
    include(BundleUtilities)
    fixup_bundle(\"${APPS}\"  \"${LIBS}\" \"${DIRS}\")
    " COMPONENT Runtime)
