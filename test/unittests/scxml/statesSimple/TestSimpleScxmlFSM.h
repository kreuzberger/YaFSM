
#define TESTFSM

 
#include "SimpleScxmlFSM.h"


class QTestSimpleScxmlFSM: public QObject
{
  Q_OBJECT
  public:
  QTestSimpleScxmlFSM()
  : mSimpleScxmlFSM()
  {
    mSimpleScxmlFSM.initFSM();
  }

  private slots:
    void initTestCase() {}
    void testInitFSM();
    void cleanupTestCase() { }

public:
  SimpleScxmlFSM mSimpleScxmlFSM;

private:
inline QTestSimpleScxmlFSM& self() {return *this;}


};
