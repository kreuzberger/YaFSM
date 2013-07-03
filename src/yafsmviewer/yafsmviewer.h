
#ifndef YaFSMVIEWER_H
#define YaFSMVIEWER_H

#include "ui_yafsmviewer.h"

class YaFsmViewer : public QWidget, public Ui::Form
{
    Q_OBJECT
public:
  YaFsmViewer(QWidget *parent = 0);
  virtual ~YaFsmViewer();
};

#endif
