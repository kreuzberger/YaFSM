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
