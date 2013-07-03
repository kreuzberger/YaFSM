#include "mainwindow.h"
#include <QtGui>
#include <iostream>

void printUsage(void)
{
  std::cerr << "yafsmviewer [options] [fsm_desprition_file]" << std::endl;
  std::cerr << "  options:" << std::endl;
  std::cerr << "    --help|-h|/?:  print help" << std::endl;
}

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);
    MainWindow *mainWindow = new MainWindow;
    QString strOpenFile;

    for(int idx = 1; idx < argc; idx++)
    {
      QString arg(argv[idx]);
      if( ("--help" == arg)
        || ("-h" == arg)
        || ("/?" == arg)
        || arg.startsWith('-')
        )
      {
        printUsage();
        return 0;
      }
      else
      {
        strOpenFile = arg;
      }
    }

    mainWindow->show();
    if(!strOpenFile.isEmpty())
    {
      mainWindow->openFile(strOpenFile);
    }

    return app.exec();
}
