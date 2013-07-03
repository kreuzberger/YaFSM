
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDateTime>

#include "yafsmviewer.h"


QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QSpinBox;
QT_END_NAMESPACE
class TreeModel;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();
  virtual ~MainWindow();
  void openFile(const QString&);

protected:
  void timerEvent(QTimerEvent *event);

private slots:
  void open();
  void about();

public slots:
  void activated(const QModelIndex & index);
  void zoomIn( void );
  void zoomOut( void );
  void zoomNormal( void );
  void zoomPercent( int );
  void urlChanged(const QUrl&);

private:
  YaFsmViewer *centralWidget;
  QMenu *fileMenu;
  QMenu *helpMenu;
  QMenu *stateViewMenu;
  QToolBar* viewToolBar;
  QAction *openAct;
  QAction *exitAct;
  QAction *aboutAct;
  QAction *aboutQtAct;
  QAction *zoomInAct;
  QAction *zoomOutAct;
  QAction *zoomNormalAct;
  QSpinBox *spinZoom;

  void createActions();
  void createMenus();
  void createToolBar();
  void readDataFromFile(const QString&);
  void readStateInfoFromFile(const QString&);
  void updateStateInfo(const QModelIndex& index, int iColumn, Qt::ItemDataRole role);

  int miUpdateTimerID;
  QDateTime mFileDateTime;
  QString mFileName;
  TreeModel* mpStateModel;
  TreeModel* mpStateInfoModel;
};

#endif
