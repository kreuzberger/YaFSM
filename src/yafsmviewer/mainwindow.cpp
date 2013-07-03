#include "mainwindow.h"
#include "treemodel.h"

#include <QtGui>
#include <QtWebKit>

MainWindow::MainWindow()
: mpStateModel(0)
, mpStateInfoModel(0)
{
  createActions();
  createMenus();
  createToolBar();
  centralWidget = new YaFsmViewer(this);
  setWindowTitle(tr("YaFSMViewer"));
  setCentralWidget(centralWidget);
  miUpdateTimerID = startTimer(1000);
  connect(centralWidget->fsmView,SIGNAL(activated( const QModelIndex & )),this,SLOT(activated( const QModelIndex& )));
  connect(centralWidget->webView,SIGNAL(urlChanged(const QUrl&)),this,SLOT(urlChanged(const QUrl&)));
  centralWidget->fsmView->setHeaderHidden(true);
  centralWidget->webView->setZoomFactor(1.0);
  centralWidget->webView->addAction(zoomInAct);
  centralWidget->webView->addAction(zoomOutAct);
  centralWidget->webView->addAction(zoomNormalAct);
}

MainWindow::~MainWindow()
{
  delete mpStateModel;
  mpStateModel = 0;
  delete mpStateInfoModel;
  mpStateInfoModel = 0;
}

void MainWindow::createActions()
{
  openAct = new QAction(QIcon(":images/standardbutton-open-16.png"),tr("&Open..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  openAct->setStatusTip(tr("Open an existing SVG file"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  exitAct = new QAction(QIcon(":images/standardbutton-close-16.png"),tr("E&xit"), this);
  exitAct->setStatusTip(tr("Exit the application"));
  exitAct->setShortcut(tr("Ctrl+Q"));
  connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

  aboutAct = new QAction(QIcon(":images/standardbutton-help-16.png"),tr("&About"), this);
  aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  aboutQtAct = new QAction(QIcon(":images/qtlogo-64.png"),tr("About &Qt"), this);
  aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

  zoomInAct = new QAction(QIcon(":/images/zoomin.png"),tr("ZoomIn"), this);
  zoomInAct->setStatusTip(tr("Zoom into state view"));
  connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

  zoomOutAct = new QAction(QIcon(":/images/zoomout.png"),tr("ZoomOut"), this);
  zoomOutAct->setStatusTip(tr("Zoom out state view"));
  connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

  zoomNormalAct = new QAction(tr("Zoom 100%"), this);
  zoomNormalAct->setStatusTip(tr("Zoom 100%"));
  connect(zoomNormalAct, SIGNAL(triggered()), this, SLOT(zoomNormal()));
}

void MainWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  stateViewMenu = menuBar()->addMenu(tr("&View"));
  stateViewMenu->addAction(zoomInAct);
  stateViewMenu->addAction(zoomOutAct);
  stateViewMenu->addAction(zoomNormalAct);

  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBar()
{
  viewToolBar = addToolBar(tr("View"));
  viewToolBar->addAction(zoomInAct);
  viewToolBar->addAction(zoomOutAct);
  spinZoom = new QSpinBox();
  spinZoom->setMinimum(1);
  spinZoom->setMaximum(400);
  spinZoom->setValue(100);
  spinZoom->setSuffix(" %");
  connect(spinZoom,SIGNAL(valueChanged(int)),this,SLOT(zoomPercent(int)));
  viewToolBar->addWidget(spinZoom);

}

void MainWindow::about()
{
  QMessageBox::about(this, tr("About YaFSMViewer"),
      tr("The <b>YaFSMViewer</b> allows simple view of fsm "
         "svg documents using a QWebView."));
}
void MainWindow::open()
{
  QString fileName = QFileDialog::getOpenFileName(this);
  openFile(fileName);
}

void MainWindow::openFile( const QString& fileName)
{
  if (!fileName.isEmpty())
  {
    if( fileName.endsWith(".txt") )
    {
      QFileInfo fileInfo(fileName);
      if(fileInfo.exists())
      {
        // change working directory into this directory due to possible
        // local filenames without path
        // read from file
        QString fileName = fileInfo.canonicalFilePath();
        qDebug() << "open file " << fileName;
        QDir::setCurrent(fileInfo.canonicalPath());
        readDataFromFile(fileName);
        mFileDateTime = QDateTime::currentDateTime();
      }
    }
    else
    {
      QMessageBox::information(this, tr("Not supported dsc file (*.txt)"),fileName);
    }
  }

}

void MainWindow::readDataFromFile(const QString& fileName)
{
  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::information(this, tr("Unable to open file"),
          file.errorString());
      return;
  }

  // todo add a form of preview (e.g. state hierarchy treeview
  setWindowTitle(QString("YaFSMViewer - %1").arg(QUrl::fromLocalFile(fileName).toString()));
  mFileName = fileName;
  delete mpStateModel;
  mpStateModel = new TreeModel();
  mpStateModel->setData(file.readAll(),3);
  centralWidget->fsmView->setModel(mpStateModel);

  centralWidget->fsmView->setColumnHidden(1,true); // fuer debug zwecke ein
  centralWidget->fsmView->setColumnHidden(2,true); // fuer debug zwecke ein
  centralWidget->webView->setUrl(QUrl("about:blank"));
  centralWidget->fsmView->setCurrentIndex(mpStateModel->index(0,1));

  emit activated(mpStateModel->index(0,1));

  file.close();
}

void MainWindow::timerEvent(QTimerEvent *event)
{
  if( 0 != event )
  {
    if( miUpdateTimerID == event->timerId())
    {
      if( !mFileName.isEmpty())
      {
        QFileInfo info(mFileName);
        if( info.exists() )
        {
          if( info.lastModified() > mFileDateTime )
          {
            if(QMessageBox::Yes == QMessageBox::question(this,tr("File DateTime change"),"file seems changed, reload?",QMessageBox::Yes|QMessageBox::No))
            {
              readDataFromFile(mFileName);
            }
            mFileDateTime = info.lastModified();
          }
        }
      }
    }
  }
}

void MainWindow::activated ( const QModelIndex & index )
{
  if (index.isValid())
  {
    const QString& fileNameStateView = mpStateModel->data(index,1, Qt::DisplayRole ).toString();
    if(!fileNameStateView.isEmpty())
    {
      QFileInfo fileInfo(fileNameStateView);
      if( fileInfo.exists())
      {
        centralWidget->webView->setUrl(QUrl::fromLocalFile(fileInfo.canonicalFilePath()));
      }
    }

    updateStateInfo(index,2,Qt::DisplayRole);
  }
}

void MainWindow::updateStateInfo(const QModelIndex& index, int iColumn, Qt::ItemDataRole role)
{
  const QString& fileNameStateInfo = mpStateModel->data(index,iColumn, role ).toString();
  if(!fileNameStateInfo.isEmpty())
  {
    QFileInfo fileInfo(fileNameStateInfo);
    if( fileInfo.exists())
    {
      readStateInfoFromFile(fileNameStateInfo);
    }
  }
}

void MainWindow::readStateInfoFromFile(const QString& fileName)
{
  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly)) {
      QMessageBox::information(this, tr("Unable to open file"),
          file.errorString());
      return;
  }

  delete mpStateInfoModel;
  mpStateInfoModel = new TreeModel();
  mpStateInfoModel->setData(file.readAll(),2);
  centralWidget->stateInfoView->setModel(mpStateInfoModel);
  centralWidget->stateInfoView->expandAll();

  file.close();
}


void MainWindow::zoomIn( void )
{
  spinZoom->stepUp();
}


void MainWindow::zoomOut( void )
{
  spinZoom->stepDown();
}

void MainWindow::zoomNormal( void )
{
  spinZoom->setValue(100);
}

void MainWindow::zoomPercent( int iPercent)
{
  centralWidget->webView->setZoomFactor(iPercent*1.0/100.0);
}

void MainWindow::urlChanged(const QUrl& url)
{
  QString strUrl = url.toLocalFile();
  //printf("url: %s\n",qPrintable(strUrl));

  QModelIndexList indexList = mpStateModel->find( mpStateModel->index(0,0),Qt::DisplayRole , strUrl);
  if( 1 == indexList.count() )
  {
    //printf("found item : %s\n",qPrintable(mpStateModel->data(indexList.at(0),1, Qt::DisplayRole ).toString()));
    centralWidget->fsmView->setCurrentIndex(indexList.at(0));
    updateStateInfo(indexList.at(0),2,Qt::DisplayRole);
  }

}


